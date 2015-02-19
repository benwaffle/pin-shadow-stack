#include <setjmp.h>

int main() {
	jmp_buf buf;
	if (!setjmp(buf))
		longjmp(buf, 1);
}
