#include "shadow_stack.h"

#include <cstring>
#include "pin.H"

#include "call_stack.h"
#include "call_frame.h"
#include "util.h"

void ShadowStack::PinTool::on_call(void *call_ins, void *target_addr, const CONTEXT *ctx)
{
	auto stack = reinterpret_cast<CallStack*>(PIN_GetContextReg(ctx, ctx_call_stack));

	CallFrame frame = {call_ins, target_addr};

	locked([&](THREADID tid){
		pr_indent(tid);
		cout << "t" << tid << ": " << frame << "\n";
	});
	indent();

	stack->push(frame);
}

void ShadowStack::PinTool::on_ret(void *ret_ins, void *ret_addr, const CONTEXT *ctx)
{
#define RET_ADDR_MATCH ({ ret_addr_is_valid(stack->pop().call_ins, ret_addr); })
	auto stack = reinterpret_cast<CallStack*>(PIN_GetContextReg(ctx, ctx_call_stack));

	unindent();

	if (!RET_ADDR_MATCH) {
		do {
			lockprf("t%d: %p " RED "skipping a frame" RESET "\n", PIN_ThreadId(), ret_addr);
			unindent();
		} while (!RET_ADDR_MATCH);
	}

	lockprf("t%d: %p: ret (to " GREEN "%p" RESET ")\n", PIN_ThreadId(), ret_ins, ret_addr);
#undef RET_ADDR_MATCH
}

void ShadowStack::PinTool::on_signal(THREADID tid, CONTEXT_CHANGE_REASON reason,
	           const CONTEXT *orig_ctx, CONTEXT *signal_ctx,
	           int32_t info, void*)
{
	if (reason == CONTEXT_CHANGE_REASON_SIGNAL) {
		auto stack = reinterpret_cast<CallStack*>(PIN_GetContextReg(signal_ctx, ctx_call_stack));

		lockprf(BLUE "t%d handling signal %d (%s)" RESET "\n", tid, info, strsignal(info));

		void **signal_ctx_sp = reinterpret_cast<void**>(PIN_GetContextReg(signal_ctx, REG_STACK_PTR));
		void *signal_ctx_ip = reinterpret_cast<void*>(PIN_GetContextReg(signal_ctx, REG_INST_PTR));

		CallFrame frame = {*signal_ctx_sp, signal_ctx_ip};
		stack->push(frame);
	}
}
