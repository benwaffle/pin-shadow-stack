#define on_call_args IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR, IARG_THREAD_ID
#define on_ret_args IARG_BRANCH_TARGET_ADDR, IARG_THREAD_ID

void on_call(void *addr, void *target_addr, THREADID tid){
	shadow[tid]->push(addr);
	if (unlikely(func_longjmp[0] == target_addr || func_longjmp[1] == target_addr))
		shadow[tid]->in_longjmp = true;
}

void on_ret(void *ret_addr, THREADID tid) {
#define RET_ADDR_MATCH ({check_ret_address(shadow[tid]->pop(), ret_addr);})
	if (unlikely(!RET_ADDR_MATCH)){
		assert(shadow[tid]->in_longjmp);
		while (likely(!RET_ADDR_MATCH))
			;
		shadow[tid]->in_longjmp = false;
	}
#undef RET_ADDR_MATCH
}