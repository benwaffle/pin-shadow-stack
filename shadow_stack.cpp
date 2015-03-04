#include "pin.H"
#include "shadow_stack.h"

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
				INS_InsertCall(tail, IPOINT_BEFORE, reinterpret_cast<AFUNPTR>(&PinTool::on_call),
					IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR, IARG_CONST_CONTEXT, IARG_END);
			else if (INS_IsRet(tail))
				INS_InsertCall(tail, IPOINT_BEFORE, reinterpret_cast<AFUNPTR>(&PinTool::on_ret),
					IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR, IARG_CONST_CONTEXT, IARG_END);
		}
	}

	void on_fork(THREADID tid, const CONTEXT*, void*)
	{
		PIN_Detach();
		locked([](THREADID){
			fprintf(stderr, "Warning: this pintool does not support fork()\n");
		});
	}

	void on_image(IMG img, void*)
	{
		auto cxx_uw_phase2 = RTN_FindByName(img, "_Unwind_RaiseException_Phase2");
		if (RTN_Valid(cxx_uw_phase2))
			PinTool::cxx_uw_phase2 = RTN_Address(cxx_uw_phase2);
	}
}

REG ShadowStack::PinTool::ctx_call_stack;
ADDRINT ShadowStack::PinTool::cxx_uw_phase2 = 0;

int main(int argc, char *argv[])
{
	// saveio();

	PIN_Init(argc, argv);
	PIN_InitSymbols();

	ShadowStack::PinTool::ctx_call_stack = PIN_ClaimToolRegister();

	PIN_InitLock(&prlock);

	PIN_AddForkFunction(FPOINT_BEFORE, ShadowStack::on_fork, nullptr);
	PIN_AddThreadStartFunction(ShadowStack::on_thread_start, nullptr);
	PIN_AddThreadFiniFunction(ShadowStack::on_thread_end, nullptr);
	PIN_AddContextChangeFunction(ShadowStack::PinTool::on_signal, nullptr);
	IMG_AddInstrumentFunction(ShadowStack::on_image, nullptr);
	TRACE_AddInstrumentFunction(ShadowStack::do_trace, nullptr);

	PIN_StartProgram();
	return 0;
}
