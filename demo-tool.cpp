#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <sstream>
#include "pin.H"
#include "util.c"

using namespace std;

//#define DEBUG

struct call {
	void *call_addr;

#ifdef DEBUG
	string caller, callee;
	void *target_addr;
	call(void *call_addr, string caller, void *target_addr, string callee) : call_addr(call_addr), caller(caller), target_addr(target_addr), callee(callee) {}

	string toString() {
		stringstream str;
		str << GREEN << call_addr << RESET " in " << caller << " ==> " << target_addr << " <" << callee << ">";
		return str.str();
	}
#else
	call(void *call_addr) : call_addr(call_addr) {}
#endif	
};

/************************************************************/
call *shadow[128];
int stacktop = 0;

void push(call *c) {
	if (stacktop < 128) shadow[stacktop++] = c;
	else fprintf(stderr, RED "Error: stack full\n" RESET);
}
call *pop() {
	if (stacktop > 0) return shadow[--stacktop];
	else return nullptr;
}
call *peek() {
	if (stacktop > 0) return shadow[stacktop - 1];
	else return nullptr;
}
/**************************************************************/

#ifdef DEBUG
int indent = 0;
void pr_indent() {
	for (int i = 0; i < indent; ++i) printf("\t");
}
#endif

bool validret(void *call_addr, void *ret_addr) {
	long diff = (char*)ret_addr - (char*)call_addr;
	return 0 < diff && diff <= 7;
}

void on_call(void *call_addr, void *target_addr){
#ifdef DEBUG
	string caller = RTN_FindNameByAddress((ADDRINT)call_addr);
    string callee = RTN_FindNameByAddress((ADDRINT)target_addr);
    
    auto frame = new call(call_addr, caller, target_addr, callee);

	pr_indent(); printf("%s\n", frame->toString().c_str());
	++indent;
#else
    auto frame = new call(call_addr);
#endif

	push(frame);
}

void on_ret(void *ret_ins, void *ret_addr){
#ifdef DEBUG
	--indent;
#endif

	call *prevframe = nullptr, *candidate = nullptr;

	while (prevframe == nullptr && stacktop > 0) {
		candidate = pop();
		if (validret(candidate->call_addr, ret_addr))
			prevframe = candidate;
		else {
			delete candidate;
#ifdef DEBUG
			pr_indent(); printf(RED "skipping a frame" RESET "\n");
			--indent;
#endif
		}
	}

	if (prevframe == nullptr) {
		fprintf(stderr, RED "Error: no valid stack frame found for %p\nstack top = %d" RESET "\n", ret_addr, stacktop);
		exit(1);
	}

#ifdef DEBUG
	pr_indent(); printf("ret at %p to " BLUE "%p" RESET "\n", ret_ins, ret_addr);
#endif

	assert(validret(candidate->call_addr, ret_addr));
}

void tr_i(TRACE tr, void*) {
    for (auto bbl = TRACE_BblHead(tr); BBL_Valid(bbl); bbl = BBL_Next(bbl)) {
        auto tail = BBL_InsTail(bbl);

        if (INS_IsCall(tail))
        	insert_call(INS, tail, on_call, IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR);
        else if (INS_IsRet(tail))
        	insert_call(INS, tail, on_ret, IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR);
    }
}

int main(int argc, char *argv[]) {
    // saveio();
    PIN_Init(argc, argv);
    PIN_InitSymbols();
    TRACE_AddInstrumentFunction(tr_i, nullptr);
    PIN_StartProgram();
    return 0;
}
