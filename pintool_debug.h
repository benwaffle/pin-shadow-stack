#define on_call_args IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR, IARG_THREAD_ID
#define on_ret_args IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR, IARG_THREAD_ID

void on_call(void *call_ins, void *target_addr, int tid){
	string caller = RTN_FindNameByAddress((ADDRINT)call_ins);
	string callee = RTN_FindNameByAddress((ADDRINT)target_addr);

	call frame(call_ins, caller, target_addr, callee);

	print("t%d: %s\n", tid, frame.str().c_str());
	indent();

	if (unlikely(func_longjmp[0] == target_addr || func_longjmp[1] == target_addr)) {
		shadow[tid]->in_longjmp = true;
		print(RED "longjmp" RESET "\n");
	}
	shadow[tid]->push(frame);
}

void on_ret(void *ret_ins, void *ret_addr, int tid){
#define RET_ADDR_MATCH ({check_ret_address(shadow[tid]->pop().call_ins, ret_addr);})
	unindent();
	if (unlikely(shadow[tid]->in_longjmp)) {
		if (!RET_ADDR_MATCH) {
			print(RED "unwinding" RESET "\n");
			do {
				print("t%d: %p " RED "skipping a frame" RESET "\n", tid, ret_addr);
				unindent();
			} while (!RET_ADDR_MATCH);
			shadow[tid]->in_longjmp = false;
		}
	} else {
		assert(RET_ADDR_MATCH);
	}
	print("t%d: %p: ret (to " GREEN "%p" RESET ")\n", tid, ret_ins, ret_addr);
#undef RET_ADDR_MATCH
}