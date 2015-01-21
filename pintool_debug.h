/*
 * This file should only be included in struct SSTool in pintool.h
 */

#define on_call_args IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR, IARG_THREAD_ID
#define on_ret_args IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR, IARG_THREAD_ID

static void on_call(void *call_ins, void *target_addr, int tid){
	string caller = RTN_FindNameByAddress((ADDRINT)call_ins);
	string callee = RTN_FindNameByAddress((ADDRINT)target_addr);

	call frame(call_ins, caller, target_addr, callee);

	print("t%d: %s\n", tid, frame.str().c_str());
	indent();

	shadow[tid].push(frame);
}

static void on_ret(void *ret_ins, void *ret_addr, int tid){
	unindent();

	while (true) {
		call prev_frame = shadow[tid].pop();
		if (check_ret_address(prev_frame.call_ins, ret_addr)) {
			break;
		} else {
			print("t%d: %p " RED "skipping a frame" RESET "\n", tid, ret_addr);
			unindent();
		}
	}

	print("t%d: %p: ret (to " GREEN "%p" RESET ")\n", tid, ret_ins, ret_addr);
}