#pragma once

#include "pin.H"
#include "call.h"
#include "util.h"

#define SHADOW_STACK_SIZE 512

struct stack {
	call frames[SHADOW_STACK_SIZE];
	int top = 0;
	bool in_longjmp = false;

	void push(call c) {
		if (likely(top < SHADOW_STACK_SIZE))
			frames[top++] = c;
		else
			die("push(): stack full");
	}

	call pop() {
		if (likely(top > 0))
			return frames[--top];
		else
			die("pop(): stack empty");
	}
};

ostream& operator<<(ostream& os, const stack& s){
	PIN_LockClient();
	for (int i = s.top; i >= 0; --i) {
#ifdef DEBUG
		void *c = s.frames[i].call_ins;
#else
		void *c = s.frames[i];
#endif
		os << c << "\t<" << RTN_FindNameByAddress((ADDRINT)c) << ">\n";
	}
	PIN_UnlockClient();
	return os;
}