#pragma once

#include "pin.H"

#include "call_stack.h"
#include "util.h"

namespace ShadowStack {
	class PinTool {
	public:
		static REG ctx_call_stack;
		static void *cxx_uw_phase2;

		static void on_call(void *call_ins, void *target_addr, const CONTEXT *ctx);
		static void on_ret(void *ret_ins, void *target_addr, const CONTEXT *ctx);
		static void on_signal(THREADID thread_id, CONTEXT_CHANGE_REASON reason,
			const CONTEXT *orig_ctx, CONTEXT *signal_ctx,
			int32_t info, void*);

		inline static bool is_return_addr(void *call_ins, void *ret_addr) {
			/*
			 Running this tool often has shown that a call instruction including
			 its operand takes up to 8 bytes. I also allow it to be 0 because when
			 signal handlers are called, call_ins is the return address taken from
			 the stack (there is no call instruction i can take the address of).
			*/
			 //TODO: lookup max possible
			auto diff = static_cast<char*>(ret_addr) - static_cast<char*>(call_ins);
			return 0 <= diff && diff <= 8;
		}
	};
}
