#ifndef _STACK_H_
#define _STACK_H_

#include "pin.H"
#include "call.h"
#include "util.h"

#define SHADOW_STACK_SIZE 512

struct stack {
	call frames[SHADOW_STACK_SIZE];
	int top = 0;

	void push(call c) {
		if (likely(top < SHADOW_STACK_SIZE))
			frames[top++] = c;
		else
			die("Error: stack full");
	}

	call pop() {
		if (likely(top > 0))
			return frames[--top];
		else
			die("Error: no valid stack frame found");
	}
};

#endif