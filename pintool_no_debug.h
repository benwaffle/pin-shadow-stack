#define on_call_args IARG_INST_PTR, IARG_THREAD_ID
#define on_ret_args IARG_BRANCH_TARGET_ADDR, IARG_THREAD_ID

void on_call(void *addr, THREADID tid){
	shadow[tid]->push(addr);
}

// go down the stack until we see the previous stack frame
// if we don't find a valid stack frame, the program exits
void on_ret(void *ret_addr, THREADID tid) {
	while (unlikely(check_ret_address(shadow[tid]->pop(), ret_addr) == false))
		;
}