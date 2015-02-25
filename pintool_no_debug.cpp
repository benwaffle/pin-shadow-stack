#include "shadow_stack.h"

#include "pin.H"

#include "call_stack.h"
#include "call_frame.h"
#include "util.h"

void ShadowStack::PinTool::on_call(void *call_ins, void *target_addr, const CONTEXT *ctx)
{
	auto stack = reinterpret_cast<CallStack*>(PIN_GetContextReg(ctx, ctx_call_stack));
	stack->push(call_ins);
}

void ShadowStack::PinTool::on_ret(void *ret_ins, void *ret_addr, const CONTEXT *ctx)
{
#define RET_ADDR_MATCH ({ ret_addr_is_valid(stack->pop(), ret_addr); })
	auto stack = reinterpret_cast<CallStack*>(PIN_GetContextReg(ctx, ctx_call_stack));
	if (unlikely(!RET_ADDR_MATCH)){
		while (likely(!RET_ADDR_MATCH))
			;
	}
#undef RET_ADDR_MATCH

}

void ShadowStack::PinTool::on_signal(THREADID tid, CONTEXT_CHANGE_REASON reason,
	           const CONTEXT *orig_ctx, CONTEXT *signal_ctx,
	           int32_t info, void*)
{
	if (reason == CONTEXT_CHANGE_REASON_SIGNAL) {
		auto stack = reinterpret_cast<CallStack*>(PIN_GetContextReg(signal_ctx, ctx_call_stack));

		void **signal_ctx_sp = reinterpret_cast<void**>(PIN_GetContextReg(signal_ctx, REG_STACK_PTR));

		stack->push(*signal_ctx_sp);
	}
}
