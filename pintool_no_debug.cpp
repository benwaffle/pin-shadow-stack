#include "shadow_stack.h"

#include "pin.H"

#include "call_stack.h"
#include "call_frame.h"
#include "util.h"

void ShadowStack::PinTool::on_call(ADDRINT call_ins, ADDRINT target_addr, CallStack *stack)
{
	stack->push(call_ins);
	if (target_addr == cxx_uw_phase2)
		stack->call_phase2 = call_ins;
}

void ShadowStack::PinTool::on_ret(ADDRINT ret_ins, ADDRINT ret_addr, CallStack *stack, ADDRINT rbp)
{
	CallFrame prev_frame;

	do prev_frame = stack->pop();
	while (unlikely( !is_return_addr(prev_frame, ret_addr) ));

	if (unlikely( stack->call_phase2 != 0 ))
		if (likely( is_return_addr(stack->call_phase2, ret_addr) )) { // returning from _Unwind_RaiseException_Phase2
			stack->call_phase2 = 0;

			ADDRINT catch_addr = *(ADDRINT*)(rbp - 0x218); // rip in catch, i.e. return address
			ADDRINT catch_func = *(ADDRINT*)(rbp - 0x1F8); // address of function containing catch

			PIN_LockClient();
			while (likely( RTN_Address(RTN_FindByAddress(stack->peek())) != catch_func ))
				stack->pop();
			PIN_UnlockClient();

			stack->push(catch_addr);
		}
}

void ShadowStack::PinTool::on_signal(THREADID tid, CONTEXT_CHANGE_REASON reason,
	           const CONTEXT *orig_ctx, CONTEXT *signal_ctx,
	           int32_t info, void*)
{
	if (likely( reason == CONTEXT_CHANGE_REASON_SIGNAL )) {
		auto stack = reinterpret_cast<CallStack*>(PIN_GetContextReg(signal_ctx, ctx_call_stack));
		auto signal_ctx_sp = reinterpret_cast<ADDRINT*>(PIN_GetContextReg(signal_ctx, REG_STACK_PTR));
		stack->push(*signal_ctx_sp);
	}
}
