#include "unistd.h"
#include "pin.H"
#include "pintool.h"

namespace ShadowStack {
	void warn_on_fork(THREADID, const CONTEXT*, void*) {
		PIN_Detach();
		fprintf(stderr, "Warning: this pintool does not support fork()\n");
	}

	void trace(TRACE tr, void*) {
		for (auto bbl = TRACE_BblHead(tr); BBL_Valid(bbl); bbl = BBL_Next(bbl)) {
			auto tail = BBL_InsTail(bbl);

			if (INS_IsCall(tail))
				INS_InsertCall(tail, IPOINT_BEFORE, (AFUNPTR)on_call, on_call_args, IARG_END);
			else if (INS_IsRet(tail))
				INS_InsertCall(tail, IPOINT_BEFORE, (AFUNPTR)on_ret, on_ret_args, IARG_END);
		}
	}

	void *func_longjmp[2] = {nullptr};
	
	void find_longjmp(IMG img, void*) {
		RTN longjmp_rtn = RTN_FindByName(img, "__longjmp");
		if (longjmp_rtn != RTN_Invalid()) {
			if (func_longjmp[0] == nullptr) func_longjmp[0] = (void*)RTN_Funptr(longjmp_rtn);
			else						    func_longjmp[1] = (void*)RTN_Funptr(longjmp_rtn);
		}
	}
}

int main(int argc, char *argv[]) {
	PIN_Init(argc, argv);
	PIN_InitSymbols();

#ifdef DEBUG
	PIN_InitLock(&prlock);
#endif

	PIN_AddForkFunction(FPOINT_BEFORE, ShadowStack::warn_on_fork, nullptr);
	PIN_AddThreadStartFunction(ShadowStack::thread_start, nullptr);
	PIN_AddThreadFiniFunction(ShadowStack::thread_end, nullptr);

	IMG_AddInstrumentFunction(ShadowStack::find_longjmp, nullptr);
	TRACE_AddInstrumentFunction(ShadowStack::trace, nullptr);

	PIN_StartProgram();
	return 0;
}
