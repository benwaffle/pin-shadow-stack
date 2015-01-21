/*
 * This file should only be included in struct SSTool in pintool.h
 */

#define on_call_args IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR
#define on_ret_args IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR

static void on_call(void *call_ins, void *target_addr){
	string caller = RTN_FindNameByAddress((ADDRINT)call_ins);
	string callee = RTN_FindNameByAddress((ADDRINT)target_addr);

	call frame(call_ins, caller, target_addr, callee);

	pr_indent(); printf("t%d: %s\n", PIN_ThreadId(), frame.str().c_str());
	++indent;

	shadow.push(frame);
}



static void on_ret(void *ret_ins, void *ret_addr){
	// if (PIN_ThreadId() != 0) return;

	--indent;

	while (true) {
		call prev_frame = shadow.pop();
		if (check_ret_address(prev_frame.call_ins, ret_addr)) {
			break;
		} else {
			pr_indent(); printf("t%d: " RED "skipping a frame" RESET "\n", PIN_ThreadId());
			--indent;
		}
	}

	pr_indent(); printf("t%d: %p: ret (to " GREEN "%p" RESET ")\n", PIN_ThreadId(), ret_ins, ret_addr);
}