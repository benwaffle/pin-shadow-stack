#pragma once

#include <string>
#include <cstring>
#include <iostream>
#include "pin.H"
#include "util.h"
#include "stack.h"
#include "call.h"

namespace ShadowStack {
	REG shadow;

	void thread_start(uint32_t tid, CONTEXT *ctx, int, void*) {
		PIN_SetContextReg(ctx, shadow, (ADDRINT)new callstack);
	}

	void thread_end(uint32_t tid, const CONTEXT *ctx, int, void*){
		delete (callstack*)PIN_GetContextReg(ctx, shadow);
	}

	static inline bool check_ret_address(void *call_ins, void *ret_addr) {
		long diff = (char*)ret_addr - (char*)call_ins;
		return 0 < diff && diff <= 8; // 8 bytes max between `call` and next instruction
	}

#ifdef DEBUG
	#include "pintool_debug.h"
#else
	#include "pintool_no_debug.h"
#endif
};
