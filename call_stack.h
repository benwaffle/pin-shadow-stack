#pragma once

#include <stack>
#include "pin.H"
#include "call_frame.h"
#include "util.h"

// proxy class to std::stack for better pop()
class CallStack
{
public:
	inline
	void push(CallFrame c) {
		frames.push(c);
	}

	inline
	CallFrame pop() {
		if (unlikely(frames.size() <= 0))
			die("pop(): stack empty");

		CallFrame top = frames.top();
		frames.pop();
		return top;
	}

	inline
	size_t size() const {
		return frames.size();
	}

private:
	std::stack<CallFrame> frames;
};
