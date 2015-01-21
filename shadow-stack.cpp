#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <sstream>
#include "pin.H"
#include "util.c"

using namespace std;

#ifdef DEBUG
	struct call {
		void *call_ins; // address of call instruction
		string caller; // calling function
		
		void *target_addr; // branch target (address)
		string callee; // function being called

		call(void *call_ins, string caller, void *target_addr, string callee):
			call_ins(call_ins), caller(caller), target_addr(target_addr), callee(callee) {}

		call(): call_ins(nullptr), caller(""), target_addr(nullptr), callee("") {}

		string str() {
			stringstream str;
			str << YELLOW << call_ins << RESET ": call "
				<< target_addr << " <" << callee << ">";
			return str.str();
		}
	};
#else
	typedef void *call;
#endif

/************* STACK ****************/
call shadow[512];
int stacktop = 0;

void push(call c) {
	if (PIN_ThreadId() != 0) return;
//	if (stacktop < 128)
		shadow[stacktop++] = c;
//	else die("Error: stack full");
}
call pop(void *x) {
	if (stacktop > 0) return shadow[--stacktop];
	else {
		cout << "[" + RTN_FindNameByAddress((ADDRINT)x) + "]\n";
		die("Error: no valid stack frame found ");
	}
}

/************* PINTOOL *************/

__attribute__((always_inline)) bool check_ret_address(void *call_ins, void *ret_addr) {
	long diff = (char*)ret_addr - (char*)call_ins;
	return 0 < diff && diff <= 7; // 7 bytes max between `call` and next instruction
}

#ifdef DEBUG
	#define on_call_args IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR

	void on_call(void *call_ins, void *target_addr){
		// get function names
		string caller = RTN_FindNameByAddress((ADDRINT)call_ins);
		string callee = RTN_FindNameByAddress((ADDRINT)target_addr);
		
		// new instance of call
		call frame(call_ins, caller, target_addr, callee);

		// print it out
		pr_indent(); printf("t%d: %s\n", PIN_ThreadId(), frame.str().c_str());
		++indent;

		// save to shadow stack
		push(frame);
	}
#else
	#define on_call push // just push a void*
	#define on_call_args IARG_INST_PTR
#endif

#ifdef DEBUG
	#define on_ret_args IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR

	void on_ret(void *ret_ins, void *ret_addr){
		if (PIN_ThreadId() != 0) return;

		--indent;

		while (true) {
			call prev_frame = pop();
			if (check_ret_address(prev_frame.call_ins, ret_addr)) {
				break;
			} else {
				pr_indent(); printf("t%d: " RED "skipping a frame" RESET "\n", PIN_ThreadId());
				--indent;
			}
		}

		pr_indent(); printf("t%d: %p: ret (to " GREEN "%p" RESET ")\n", PIN_ThreadId(), ret_ins, ret_addr);
	}
#else
	#define on_ret_args IARG_BRANCH_TARGET_ADDR
	// go down the stack until we see the previous stack frame
	// if we don't find a valid stack frame, the program exits
	void on_ret(void *ret_addr) {
		if (PIN_ThreadId() != 0) return;

		int i = 0;
		while (true){
			auto p = pop(ret_addr);
			if (check_ret_address(p, ret_addr))
				break;
			else
				++i;
		}
		if (i) printf("%d frames skipped\n", i);

		// while (!check_ret_address(pop(), ret_addr))
		// 	;
	}
#endif

void trace(TRACE tr, void*) {
	for (auto bbl = TRACE_BblHead(tr); BBL_Valid(bbl); bbl = BBL_Next(bbl)) {
		auto tail = BBL_InsTail(bbl);

		if (INS_IsCall(tail))
			INS_InsertCall(tail, IPOINT_BEFORE, (AFUNPTR)on_call, on_call_args, IARG_END);
		else if (INS_IsRet(tail))
			INS_InsertCall(tail, IPOINT_BEFORE, (AFUNPTR)on_ret, on_ret_args, IARG_END);
	}
}

int main(int argc, char *argv[]) {
	PIN_Init(argc, argv);
	PIN_InitSymbols();
	TRACE_AddInstrumentFunction(trace, nullptr);
	PIN_StartProgram();
	return 0;
}
