#ifndef _PINTOOL_H_
#define _PINTOOL_H_

#include <string>
#include "pin.H"
#include "util.h"
#include "stack.h"
#include "call.h"

using namespace std;

struct SSTool {
	static stack shadow;

	__attribute__((always_inline))
	static bool check_ret_address(void *call_ins, void *ret_addr) {
		long diff = (char*)ret_addr - (char*)call_ins;
		return 0 < diff && diff <= 7; // 7 bytes max between `call` and next instruction
	}

#ifdef DEBUG
	#include "pintool_debug.h"
#else
	#include "pintool_nodebug.h"
#endif
};

stack SSTool::shadow;

#endif