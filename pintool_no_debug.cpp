#include "shadow_stack.h"

#include "pin.H"

#include "call_stack.h"
#include "call_frame.h"
#include "util.h"

void ShadowStack::PinTool::on_call(void *call_ins, void *target_addr, const CONTEXT *ctx)
{
	auto stack = reinterpret_cast<CallStack*>(PIN_GetContextReg(ctx, ctx_call_stack));

	stack->push(call_ins);

	if (target_addr == cxx_uw_phase2)
		stack->call_phase2 = call_ins;
}

void ShadowStack::PinTool::on_ret(void *ret_ins, void *ret_addr, const CONTEXT *ctx)
{
	auto stack = reinterpret_cast<CallStack*>(PIN_GetContextReg(ctx, ctx_call_stack));

	CallFrame prev_frame;

	while(prev_frame = stack->pop(),
		unlikely( false == is_return_addr(prev_frame, ret_addr) ))
		;

	if (unlikely( stack->call_phase2 != nullptr ))
		if (likely( is_return_addr(stack->call_phase2, ret_addr) )) { // returning from _Unwind_RaiseException_Phase2
			stack->call_phase2 = nullptr;

			ADDRINT rbp = PIN_GetContextReg(ctx, REG_RBP); // rbp
			void *catch_addr = *(void**)(rbp - 0x218); // rip in catch, i.e. return address
			void *catch_func = *(void**)(rbp - 0x1F8); // address of function containing catch

			PIN_LockClient();
			while (likely( // find shadow stack frame of the function catching the exception
				RTN_Address(RTN_FindByAddress( (ADDRINT)stack->peek() )) != (ADDRINT)catch_func
				))
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

		void **signal_ctx_sp = reinterpret_cast<void**>(PIN_GetContextReg(signal_ctx, REG_STACK_PTR));

		stack->push(*signal_ctx_sp);
	}
}
