#include "shadow_stack.h"

#include <cstring>
#include "pin.H"

#include "call_stack.h"
#include "call_frame.h"
#include "util.h"

void ShadowStack::PinTool::on_call(
	const ADDRINT call_ins,
	const ADDRINT target_addr,
	CallStack *stack)
{
	CallFrame frame = {call_ins, target_addr};
	stack->push(frame);

	locked([&](THREADID tid){
		pr_indent(tid);
		cout << "t" << tid << ": " << frame << "\n";
	});
	indent();
}

void ShadowStack::PinTool::on_ret(
	const ADDRINT ret_ins,
	const ADDRINT ret_addr,
	CallStack *stack)
{
	unindent();

	while(!is_return_addr(stack->pop().call_ins, ret_addr)) {
			lockprf("t%d: " RED "skipping a frame" RESET "\n", PIN_ThreadId());
			unindent();
	}

	lockprf("t%d: %p: ret (to " GREEN "%p" RESET ")\n", PIN_ThreadId(), (void*)ret_ins, (void*)ret_addr);
}

void ShadowStack::PinTool::on_signal(THREADID tid, CONTEXT_CHANGE_REASON reason,
	const CONTEXT *orig_ctx, CONTEXT *signal_ctx, int32_t info, void*)
{
	if (likely( reason == CONTEXT_CHANGE_REASON_SIGNAL )) {
		auto stack = (CallStack*)(PIN_GetContextReg(signal_ctx, ctx_call_stack));

		lockprf(BLUE "t%d: client program received signal %d (%s)" RESET "\n", tid, info, strsignal(info));

		auto signal_ctx_sp = (ADDRINT*)(PIN_GetContextReg(signal_ctx, REG_STACK_PTR));
		auto signal_ctx_ip = PIN_GetContextReg(signal_ctx, REG_INST_PTR);

		CallFrame frame = {*signal_ctx_sp, signal_ctx_ip};
		stack->push(frame);
	} else if (reason == CONTEXT_CHANGE_REASON_FATALSIGNAL) {
		lockprf(RED "t%d: client program received fatal signal %d (%s)" RESET "\n", tid, info, strsignal(info));
	} else {
		lockprf(BLUE "t%d: unknown context change (%d)" RESET "\n", tid, (int)reason);
	}
}

void ShadowStack::PinTool::on_call_phase2(CallStack *stack, _Unwind_Context *uw)
{
	stack->handler_ctx = uw;
}

void ShadowStack::PinTool::on_ret_phase2(CallStack *stack)
{
	PIN_LockClient();

	ADDRINT catch_addr = ADDRINT(_Unwind_GetIP(stack->handler_ctx)); // IP in catch, i.e. return address
	ADDRINT catch_func = RTN_Address(RTN_FindByAddress(catch_addr)); // address of function containing catch

	CallFrame top_frame_copy = stack->pop(); // IPOINT_AFTER is right at the ret instruction, so save the top frame

	while (likely( RTN_Address(RTN_FindByAddress(stack->peek().call_ins)) != catch_func ))
	{
		stack->pop();
		unindent();
	}

	CallFrame handler = { catch_addr, 0 };
	stack->push(handler);
	stack->push(top_frame_copy);

	PIN_UnlockClient();

	lockprf(BLUE "t%d: catch handler @ %p in %p <%s>" RESET "\n", PIN_ThreadId(),
		(void*)catch_addr, (void*)catch_func, RTN_FindNameByAddress(ADDRINT(catch_func)).c_str());
}