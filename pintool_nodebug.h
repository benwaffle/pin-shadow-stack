/*
 * This file should only be included in struct SSTool in pintool.h
 */

#define on_call_args IARG_INST_PTR
#define on_ret_args IARG_BRANCH_TARGET_ADDR

static void on_call(void *addr){
	shadow.push(addr);
}

// go down the stack until we see the previous stack frame
// if we don't find a valid stack frame, the program exits
static void on_ret(void *ret_addr) {
	while (!check_ret_address(shadow.pop(), ret_addr))
		;
}