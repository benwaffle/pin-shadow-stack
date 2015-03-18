#include "shadow_stack.h"

#include "pin.H"

#include "call_stack.h"
#include "call_frame.h"
#include "util.h"

void ShadowStack::on_call(const ADDRINT call_ins, CallStack *stack)
{
	stack->push(call_ins);
}

void ShadowStack::on_ret(const ADDRINT ret_addr, CallStack *stack)
{
	while (unlikely( !is_return_addr(stack->pop(), ret_addr) ))
		;
}

void ShadowStack::on_signal(THREADID tid, CONTEXT_CHANGE_REASON reason,
	const CONTEXT *orig_ctx, CONTEXT *signal_ctx, int32_t info, void*)
{
	if (likely( reason == CONTEXT_CHANGE_REASON_SIGNAL )) {
		auto stack = (CallStack*)(PIN_GetContextReg(signal_ctx, ctx_call_stack));
		auto signal_ctx_sp = (ADDRINT*)(PIN_GetContextReg(signal_ctx, REG_STACK_PTR));
		stack->push(*signal_ctx_sp);
	}
}

void ShadowStack::on_call_phase2(CallStack *stack, _Unwind_Context *uw)
{
	stack->handler_ctx = uw;
}

void ShadowStack::on_ret_phase2(CallStack *stack)
{
	PIN_LockClient();

	ADDRINT catch_addr = ADDRINT(_Unwind_GetIP(stack->handler_ctx)); // IP in catch, i.e. return address
	ADDRINT catch_func = RTN_Address(RTN_FindByAddress(catch_addr)); // address of function containing catch

	CallFrame top_frame_copy = stack->pop(); // IPOINT_AFTER is right at the ret instruction, so save the top frame

	while (likely( RTN_Address(RTN_FindByAddress(stack->peek())) != catch_func ))
	{
		stack->pop();
	}

	stack->push(catch_addr);
	stack->push(top_frame_copy);

	PIN_UnlockClient();
}