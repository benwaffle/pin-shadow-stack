#include <unwind.h>

#include "pin.H"
#include "shadow_stack.h"

namespace ShadowStack
{
	TLS_KEY tls_call_stack;

	void on_thread_start(THREADID tid, CONTEXT*, int, void*)
	{
		PIN_SetThreadData(tls_call_stack, new CallStack, tid);
	}

	void do_trace(TRACE tr, void*)
	{
		auto tail = BBL_InsTail(TRACE_BblTail(tr));
		if (INS_IsCall(tail))
		{
			INS_InsertCall(tail, IPOINT_BEFORE, AFUNPTR(&on_call), on_call_args, IARG_END);
		}
		else if (INS_IsRet(tail))
		{
			INS_InsertCall(tail, IPOINT_BEFORE, AFUNPTR(&on_ret), on_ret_args, IARG_END);
		}
	}

	void find_cxx_phase2(IMG img, void*)
	{
		/*
		when _Unwind_RaiseException_Phase2 returns, its first argument, an _Unwind_Context *,
		contains the IP of the exception handler
		*/
		auto rtn = RTN_FindByName(img, "_Unwind_RaiseException_Phase2");
		if (RTN_Valid(rtn))
		{
			RTN_Open(rtn);

			RTN_InsertCall(rtn, IPOINT_BEFORE,
				AFUNPTR(&on_call_phase2),
				IARG_THREAD_ID,
				IARG_FUNCARG_ENTRYPOINT_VALUE, 1, // _Unwind_Context*
				IARG_END);

			RTN_InsertCall(rtn, IPOINT_AFTER,
				AFUNPTR(&on_ret_phase2),
				IARG_THREAD_ID,
				IARG_END);

			RTN_Close(rtn);
		}
	}
}

int main(int argc, char *argv[])
{
	// saveio();

	// use when we print like crazy (eg gtk)
	// setvbuf(stdout, nullptr, _IOFBF, 4096);

	PIN_Init(argc, argv);
	PIN_InitSymbols();

	ShadowStack::tls_call_stack = PIN_CreateThreadDataKey(&CallStack::destroy);

	PIN_InitLock(&prlock);

	PIN_AddThreadStartFunction(ShadowStack::on_thread_start, nullptr);
	PIN_AddContextChangeFunction(ShadowStack::on_signal, nullptr);
	IMG_AddInstrumentFunction(ShadowStack::find_cxx_phase2, nullptr);
	TRACE_AddInstrumentFunction(ShadowStack::do_trace, nullptr);

	PIN_StartProgram();

	return 0;
}
