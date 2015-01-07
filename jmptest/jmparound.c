#include <stdio.h>
#include <setjmp.h>

jmp_buf buf;

void aux() {
	puts("aux()");
	longjmp(buf, 1);
	puts("longjmp() failed");
}

int main() {
	puts("welcome...");
	if (setjmp(buf) == 0) {
		puts("setjmp() saved stack");
		aux();
	} else {
		puts("longjmp() restored stack");
	}
	puts("bye");
}

