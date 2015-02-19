#define on_call_args IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR, IARG_CONST_CONTEXT
#define on_ret_args  IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR, IARG_CONST_CONTEXT

void on_call(void *call_ins, void *target_addr, const CONTEXT *ctx)
{
	auto st = (callstack*)PIN_GetContextReg(ctx, shadow);

	if (RTN_FindNameByAddress((ADDRINT)target_addr) == "_dl_fixup") {
		// st->pltsetup = call_ins;
		lockprf("t%d: _dl_fixup()\n", PIN_ThreadId());
	}

	call frame(call_ins, target_addr);

	if (st->pltsetup == 0) {
		locked([&](THREADID tid){
			pr_indent(tid);
			cout << "t" << tid << ": " << frame << "\n";
		});
		indent();
	}

	st->push(frame);
}

void on_ret(void *ret_ins, void *ret_addr, const CONTEXT *ctx)
{
#define RET_ADDR_MATCH ({ check_ret_address(st->pop().call_ins, ret_addr); })
	auto st = (callstack*)PIN_GetContextReg(ctx, shadow);
	
	if (st->pltsetup == 0) unindent();

	// if (st->ignore){
		// st->ignore = false;
		// return;
	// }

	if (!RET_ADDR_MATCH) {
		do {
			if (st->pltsetup == 0) lockprf("t%d: %p " RED "skipping a frame" RESET "\n", PIN_ThreadId(), ret_addr);
			if (st->pltsetup == 0) unindent();
		} while (!RET_ADDR_MATCH);
	}
	
	if (st->pltsetup == 0)
		lockprf("t%d: %p: ret (to " GREEN "%p" RESET ")\n", PIN_ThreadId(), ret_ins, ret_addr);
	else
		if (check_ret_address(st->pltsetup, ret_addr))
			st->pltsetup = 0;
#undef RET_ADDR_MATCH
}

void on_signal(THREADID tid,
	           CONTEXT_CHANGE_REASON reason,
	           const CONTEXT *from,
	           CONTEXT *to,
	           int32_t info,
	           void*)
{
	if (reason == CONTEXT_CHANGE_REASON_SIGNAL) {
		auto st = (callstack*)PIN_GetContextReg(from, shadow);
		st->ignore = true;
		lockprf(BLUE "t%d handling signal %d (%s)" RESET "\n", tid, info, strsignal(info));
		ADDRINT brsp = PIN_GetContextReg(from, REG_STACK_PTR),
				arsp = PIN_GetContextReg(to  , REG_STACK_PTR);
		lockprf("stack top = %p, then %p\n", *(void**)brsp, *(void**)arsp);
	}
}