#include "pin.H"
#include "shadow_stack.h"
#include <unwind.h>

namespace ShadowStack
{
	void on_thread_start(uint32_t thread_id, CONTEXT *context, int, void*)
	{
		PIN_SetContextReg(context, PinTool::ctx_call_stack, reinterpret_cast<ADDRINT>(new CallStack));
	}

	void on_thread_end(uint32_t thread_id, const CONTEXT *context, int, void*)
	{
		delete reinterpret_cast<CallStack*>(PIN_GetContextReg(context, PinTool::ctx_call_stack));
	}

	void do_trace(TRACE tr, void*)
	{
		for (auto bbl = TRACE_BblHead(tr); BBL_Valid(bbl); bbl = BBL_Next(bbl)) {
			auto tail = BBL_InsTail(bbl);

			if (INS_IsCall(tail))
				INS_InsertCall(tail, IPOINT_BEFORE, AFUNPTR(&PinTool::on_call), on_call_args, IARG_END);
			else if (INS_IsRet(tail))
				INS_InsertCall(tail, IPOINT_BEFORE, AFUNPTR(&PinTool::on_ret), on_ret_args, IARG_END);
		}
	}

	void on_fork(THREADID tid, const CONTEXT*, void*)
	{
		PIN_Detach();
		locked([](THREADID){
			fprintf(stderr, "Warning: this pintool does not support fork()\n");
		});
	}

	void find_cxx_phase2(RTN rtn, void*)
	{
		if (RTN_Name(rtn) == "_Unwind_RaiseException_Phase2")
		{
			RTN_Open(rtn);
			RTN_InsertCall(rtn, IPOINT_BEFORE, AFUNPTR(&PinTool::on_call_phase2),
				IARG_REG_VALUE, PinTool::ctx_call_stack, IARG_FUNCARG_ENTRYPOINT_VALUE, 1, IARG_END);
			RTN_InsertCall(rtn, IPOINT_AFTER, AFUNPTR(&PinTool::on_ret_phase2),
				IARG_REG_VALUE, PinTool::ctx_call_stack, IARG_END);
			RTN_Close(rtn);
		}
	}
}

REG ShadowStack::PinTool::ctx_call_stack;

int main(int argc, char *argv[])
{
	// saveio();

	// use when we print like crazy (eg gtk)
	// setvbuf(stdout, nullptr, _IOFBF, 4096);

	PIN_Init(argc, argv);
	PIN_InitSymbols();

	ShadowStack::PinTool::ctx_call_stack = PIN_ClaimToolRegister();

	PIN_InitLock(&prlock);

	PIN_AddForkFunction(FPOINT_BEFORE, ShadowStack::on_fork, nullptr);
	PIN_AddThreadStartFunction(ShadowStack::on_thread_start, nullptr);
	PIN_AddThreadFiniFunction(ShadowStack::on_thread_end, nullptr);
	PIN_AddContextChangeFunction(ShadowStack::PinTool::on_signal, nullptr);
	RTN_AddInstrumentFunction(ShadowStack::find_cxx_phase2, nullptr);
	TRACE_AddInstrumentFunction(ShadowStack::do_trace, nullptr);

	PIN_StartProgram();
	return 0;
}
