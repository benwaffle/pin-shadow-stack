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
	stack->push(frame);

	if (target_addr == cxx_uw_phase2)
		stack->call_phase2 = call_ins;

	locked([&](THREADID tid){
		pr_indent(tid);
		cout << "t" << tid << ": " << frame << "\n";
	});
	indent();
}

void ShadowStack::PinTool::on_ret(void *ret_ins, void *ret_addr, const CONTEXT *ctx)
{
	const int tid = PIN_ThreadId();
	unindent();
	auto stack = reinterpret_cast<CallStack*>(PIN_GetContextReg(ctx, ctx_call_stack));

	CallFrame prev_frame;
	while(prev_frame = stack->pop(),
		unlikely( false == is_return_addr(prev_frame.call_ins, ret_addr) )) {
			lockprf("t%d: " RED "skipping a frame" RESET "\n", tid);
			unindent();
	}

	if (unlikely( stack->call_phase2 != nullptr ))
		if (likely( is_return_addr(stack->call_phase2, ret_addr) )) { // returning from _Unwind_RaiseException_Phase2
			stack->call_phase2 = nullptr;

			ADDRINT rbp = PIN_GetContextReg(ctx, REG_RBP); // rbp
			void *catch_addr = *(void**)(rbp - 0x218); // rip in catch, i.e. return address
			void *catch_func = *(void**)(rbp - 0x1F8); // address of function containing catch

			lockprf(BLUE "catch handler @ %p in %s" RESET "\n", catch_addr, RTN_FindNameByAddress((ADDRINT)catch_func).c_str());

			while (likely( stack->peek().target_addr != catch_func )) {
				stack->pop();
				unindent();
			}

			CallFrame handler = {catch_addr, catch_addr}; // fake call_ins, target_addr doesn't matter
			stack->push(handler);
		}

	lockprf("t%d: %p: ret (to " GREEN "%p" RESET ")\n", tid, ret_ins, ret_addr);
}

void ShadowStack::PinTool::on_signal(THREADID tid, CONTEXT_CHANGE_REASON reason,
	           const CONTEXT *orig_ctx, CONTEXT *signal_ctx,
	           int32_t info, void*)
{
	if (likely( reason == CONTEXT_CHANGE_REASON_SIGNAL )) {
		auto stack = reinterpret_cast<CallStack*>(PIN_GetContextReg(signal_ctx, ctx_call_stack));

		lockprf(BLUE "t%d: client program received signal %d (%s)" RESET "\n", tid, info, strsignal(info));

		void **signal_ctx_sp = reinterpret_cast<void**>(PIN_GetContextReg(signal_ctx, REG_STACK_PTR));
		void *signal_ctx_ip = reinterpret_cast<void*>(PIN_GetContextReg(signal_ctx, REG_INST_PTR));

		CallFrame frame = {*signal_ctx_sp, signal_ctx_ip};
		stack->push(frame);
	} else if (reason == CONTEXT_CHANGE_REASON_FATALSIGNAL) {
		lockprf(RED "t%d: client program received fatal signal %d (%s)" RESET "\n", tid, info, strsignal(info));
	} else {
		lockprf(BLUE "t%d: unknown context change %d" RESET "\n", tid, (int)reason);
	}
}
