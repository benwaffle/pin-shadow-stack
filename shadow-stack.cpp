#include "pin.H"
#include "pintool.h"

void trace(TRACE tr, void*) {
	for (auto bbl = TRACE_BblHead(tr); BBL_Valid(bbl); bbl = BBL_Next(bbl)) {
		auto tail = BBL_InsTail(bbl);

		if (INS_IsCall(tail))
			INS_InsertCall(tail, IPOINT_BEFORE, (AFUNPTR)SSTool::on_call, on_call_args, IARG_END);
		else if (INS_IsRet(tail))
			INS_InsertCall(tail, IPOINT_BEFORE, (AFUNPTR)SSTool::on_ret, on_ret_args, IARG_END);
	}
}

int main(int argc, char *argv[]) {
	PIN_Init(argc, argv);
	PIN_InitSymbols();
	TRACE_AddInstrumentFunction(trace, nullptr);
	PIN_StartProgram();
	return 0;
}
