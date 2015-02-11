#define on_call_args IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR, IARG_THREAD_ID
#define on_ret_args IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR, IARG_THREAD_ID

void on_call(void *call_ins, void *target_addr, THREADID tid){
	call frame(call_ins, target_addr);

	locked(tid, [&](){
		pr_indent(tid);
		cout << "t" << tid << ": " << frame << "\n";
	});
	
	indent();

	if (unlikely(func_longjmp[0] == target_addr || func_longjmp[1] == target_addr)) {
		shadow[tid]->in_longjmp = true;
		lockprf(tid, RED "longjmp" RESET "\n");
	}
	shadow[tid]->push(frame);
}

void on_ret(void *ret_ins, void *ret_addr, THREADID tid){
#define RET_ADDR_MATCH ({check_ret_address(shadow[tid]->pop().call_ins, ret_addr);})
	unindent();
	if (unlikely(shadow[tid]->in_longjmp)) {
		if (!RET_ADDR_MATCH) {
// REMOVE_THIS:
			lockprf(tid, RED "unwinding" RESET "\n");
			do {
				lockprf(tid, "t%d: %p " RED "skipping a frame" RESET "\n", tid, ret_addr);
				unindent();
			} while (!RET_ADDR_MATCH);
			shadow[tid]->in_longjmp = false;
		}
	} else {
		bool val = RET_ADDR_MATCH;
		if (!val) {
			locked(tid, [&](){
				cout << *shadow[tid] << "\n";
			});
			assert(val);
			// goto REMOVE_THIS;
		}
	}
	lockprf(tid, "t%d: %p: ret (to " GREEN "%p" RESET ")\n", tid, ret_ins, ret_addr);
#undef RET_ADDR_MATCH
}