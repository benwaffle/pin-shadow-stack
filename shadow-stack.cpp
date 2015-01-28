#include "unistd.h"
#include "pin.H"
#include "pintool.h"

namespace SS = ShadowStack;

void warn_on_fork(THREADID, const CONTEXT*, void*) {
	PIN_Detach();
	fprintf(stderr, "Warning: this pintool does not support fork()\n");
}

void trace(TRACE tr, void*) {
	for (auto bbl = TRACE_BblHead(tr); BBL_Valid(bbl); bbl = BBL_Next(bbl)) {
		auto tail = BBL_InsTail(bbl);

		if (INS_IsCall(tail))
			INS_InsertCall(tail, IPOINT_BEFORE, (AFUNPTR)SS::on_call, on_call_args, IARG_END);
		else if (INS_IsRet(tail))
			INS_InsertCall(tail, IPOINT_BEFORE, (AFUNPTR)SS::on_ret, on_ret_args, IARG_END);
	}
}

int main(int argc, char *argv[]) {
	PIN_Init(argc, argv);
	PIN_InitSymbols();

#ifdef DEBUG
	PIN_InitLock(&prlock);
#endif

	PIN_AddForkFunction(FPOINT_BEFORE, warn_on_fork, nullptr);
	PIN_AddThreadStartFunction(SS::thread_start, nullptr);
	PIN_AddThreadFiniFunction(SS::thread_end, nullptr);
	
	TRACE_AddInstrumentFunction(trace, nullptr);
	PIN_StartProgram();
	return 0;
}
