#pragma once

#include <unwind.h>
#include <stack>
#include "pin.H"

#include "call_frame.h"
#include "util.h"

class CallStack
{
public:
	_Unwind_Context *handler_ctx = nullptr;

	inline
	void push(CallFrame c) {
		frames.push(c);
	}

	inline
	CallFrame pop() {
		if (unlikely(frames.size() <= 0))
			die("pop(): stack empty");

		auto top = frames.top();
		frames.pop();
		return top;
	}

	inline
	CallFrame peek() const {
		return frames.top();
	}

	inline
	size_t size() const {
		return frames.size();
	}

	void trace() const {
		PIN_LockClient();

		std::stack<CallFrame> copy(frames);
		for (int i = 0; copy.size() != 0; ++i) {
			ADDRINT call = copy.top()
#ifdef DEBUG
				.call_ins
#endif
				;
			copy.pop();

			int32_t lineno;
			string filename;
			PIN_GetSourceLocation(call, nullptr, &lineno, &filename);

			locked([&](THREADID tid){
				printf("#%-2d 0x%016lx in %s () ", i, call, RTN_FindNameByAddress(call).c_str());
				if (filename == "")
					printf("from %s\n", IMG_Name(IMG_FindByAddress(call)).c_str());
				else
					printf("at %s:%d\n", filename.c_str(), lineno);
			});
		}

		PIN_UnlockClient();
	}

	static void destroy(void *cs)
	{
		delete (CallStack*)cs;
	}

private:
	std::stack<CallFrame> frames;
};
