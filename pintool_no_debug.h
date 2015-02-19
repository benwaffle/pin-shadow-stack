#define on_call_args IARG_FAST_ANALYSIS_CALL, IARG_INST_PTR, /*IARG_BRANCH_TARGET_ADDR,*/ IARG_CONST_CONTEXT
#define on_ret_args  IARG_FAST_ANALYSIS_CALL, IARG_BRANCH_TARGET_ADDR, IARG_CONST_CONTEXT

void PIN_FAST_ANALYSIS_CALL on_call(void *addr, /*void *target_addr,*/ const CONTEXT *ctx){
	stack *st = (stack*)PIN_GetContextReg(ctx, shadow);
	st->push(addr);
}

void PIN_FAST_ANALYSIS_CALL on_ret(void *ret_addr, const CONTEXT *ctx) {
#define RET_ADDR_MATCH ({check_ret_address(st->pop(), ret_addr);})
        stack *st = (stack*)PIN_GetContextReg(ctx, shadow);
	if (unlikely(!RET_ADDR_MATCH)){
		while (likely(!RET_ADDR_MATCH))
			;
	}
#undef RET_ADDR_MATCH
}

void on_signal(THREADID tid,
	           CONTEXT_CHANGE_REASON reason,
	           const CONTEXT *from,
	           const CONTEXT *to,
	           int32_t info,
	           void*)
{
}