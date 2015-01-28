#ifndef _PINTOOL_H_
#define _PINTOOL_H_

#include <string>
#include <map>
#include "pin.H"
#include "util.h"
#include "stack.h"
#include "call.h"

using std::string;

namespace ShadowStack {
	std::map<uint32_t,stack*> shadow; // thread id -> stack

	void thread_start(uint32_t tid, CONTEXT*, int, void*) {
		shadow[tid] = new stack;
	}

	void thread_end(uint32_t tid, const CONTEXT*, int, void*){
		delete shadow[tid];
		shadow.erase(tid);
	}

	bool check_ret_address(void *call_ins, void *ret_addr) {
		long diff = (char*)ret_addr - (char*)call_ins;
		return 0 < diff && diff <= 8; // 8 bytes max between `call` and next instruction
	}

#ifdef DEBUG
	#include "pintool_debug.h"
#else
	#include "pintool_no_debug.h"
#endif
};

#endif
