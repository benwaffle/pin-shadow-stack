#include <cstdio>

void thrower() {
	throw 1234;
}

void thrower2() {
	throw 1234;
}

void dummy2() {
	thrower();
}

void dummy1() {
	dummy2();
}

void catcher() {
	try {
		dummy1();
	} catch (...) {
	}
}

int main() {
	puts("call safe func");
	catcher();

	puts("try/catch unsafe func");
	try { thrower(); }
	catch (...) {}

	puts("call unsafe func");
	thrower2();
}
