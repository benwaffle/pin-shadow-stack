#pragma once

#include <unwind.h> // struct _Unwind_Context and helper functions

#include "pin.H"

#include "call_stack.h"
#include "util.h"

namespace ShadowStack {
	class PinTool {
	public:
		static REG ctx_call_stack;

#ifdef DEBUG
		#define on_call_args IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR, IARG_REG_VALUE, PinTool::ctx_call_stack
		#define on_ret_args  IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR, IARG_REG_VALUE, PinTool::ctx_call_stack
		static void on_call(ADDRINT, ADDRINT, CallStack *);
		static void on_ret(ADDRINT, ADDRINT, CallStack *);
#else
		#define on_call_args IARG_INST_PTR,          IARG_REG_VALUE, PinTool::ctx_call_stack
		#define on_ret_args IARG_BRANCH_TARGET_ADDR, IARG_REG_VALUE, PinTool::ctx_call_stack
		static void on_call(ADDRINT, CallStack *);
		static void on_ret(ADDRINT, CallStack *);
#endif

		static void on_signal(THREADID thread_id, CONTEXT_CHANGE_REASON reason,
			const CONTEXT *orig_ctx, CONTEXT *signal_ctx,
			int32_t info, void*);

		// C++ (Itanium ABI) exception handling
		static void on_call_phase2(CallStack *, _Unwind_Context *);
		static void on_ret_phase2(CallStack *);

		inline
		static bool is_return_addr(ADDRINT call_ins, ADDRINT ret_addr) {
			/*
			 Running this tool often has shown that a call instruction including
			 its operand takes up to 8 bytes. I also allow it to be 0 because when
			 signal handlers are called, call_ins is the return address taken from
			 the stack (there is no call instruction i can take the address of).
			*/
			 //TODO: lookup max possible
			auto diff = ret_addr - call_ins;
			return 0 <= diff && diff <= 8;
		}
	};
}
