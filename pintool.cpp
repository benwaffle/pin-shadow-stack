#include "shadow_stack.h"

#include "pin.H"

#include "call_stack.h"
#include "call_frame.h"
#include "util.h"

void ShadowStack::on_call(const ADDRINT call_ins, THREADID tid)
{
	CallStack *stack = (CallStack*)PIN_GetThreadData(tls_call_stack, tid);
	stack->push(call_ins);
}

void ShadowStack::on_ret(const ADDRINT ret_addr, THREADID tid)
{
	CallStack *stack = (CallStack*)PIN_GetThreadData(tls_call_stack, tid);
	while (unlikely( !is_return_addr(stack->pop(), ret_addr) ))
		;
}

void ShadowStack::on_signal(THREADID tid, CONTEXT_CHANGE_REASON reason,
	const CONTEXT *orig_ctx, CONTEXT *signal_ctx, int32_t info, void*)
{
	if (likely( reason == CONTEXT_CHANGE_REASON_SIGNAL )) {
		CallStack *stack = (CallStack*)PIN_GetThreadData(tls_call_stack, tid);
		auto signal_ctx_sp = (ADDRINT*)(PIN_GetContextReg(signal_ctx, REG_STACK_PTR));
		stack->push(*signal_ctx_sp);
	}
}

void ShadowStack::on_call_phase2(THREADID tid, _Unwind_Context *uw)
{
	CallStack *stack = (CallStack*)PIN_GetThreadData(tls_call_stack, tid);
	stack->handler_ctx = uw;
}

void ShadowStack::on_ret_phase2(THREADID tid)
{
	CallStack *stack = (CallStack*)PIN_GetThreadData(tls_call_stack, tid);
	PIN_LockClient();

	CallFrame top_frame_copy = stack->pop(); // IPOINT_AFTER is right at the ret instruction, so save the top frame
	ADDRINT catch_addr = ADDRINT(_Unwind_GetIP(stack->handler_ctx)); // IP in catch, i.e. return address
	stack->push(catch_addr);
	stack->push(top_frame_copy);

	PIN_UnlockClient();
}
