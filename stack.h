#pragma once

#include <stack>
#include "pin.H"
#include "call.h"
#include "util.h"

struct callstack {
	void *pltsetup = 0;
	bool ignore;
	stack<call> frames;

	inline void push(call c) {
		frames.push(c);
	}

	inline call pop() {
		if (unlikely(frames.size() <= 0))
			die("pop(): stack empty");

		call top = frames.top();
		frames.pop();
		return top;
	}

	inline size_t size() const {
		return frames.size();
	}
};

ostream& operator<<(ostream& os, callstack& st){
	PIN_LockClient();
	while (st.size() != 0) {
#ifdef DEBUG
		void *c = st.pop().call_ins;
#else
		void *c = st.pop();
#endif
		os << c << "\t<" << RTN_FindNameByAddress((ADDRINT)c) << ">\n";
	}
	PIN_UnlockClient();
	return os;
}
