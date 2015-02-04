#ifndef _UTIL_H_
#define _UTIL_H_

#include <string>
#include <cassert>

using namespace std;

// terminal colors
#define RED    "\e[41m"
#define GREEN  "\e[32m"
#define YELLOW "\e[33m"
#define BLUE   "\e[34m"
#define PURPLE "\e[35m"
#define RESET  "\e[0m"

#define likely(x) __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)

// logging

#ifdef DEBUG
	extern int numtabs[128];
	extern PIN_LOCK prlock;

	void pr_indent(int tid);
	// TODO: avoid cluttered output from multiple threads (print to file?)
	// TODO: no lock on single thread
	#define print(...) ({ \
		PIN_GetLock(&prlock, tid); \
		pr_indent(tid); \
		printf(__VA_ARGS__); \
		PIN_ReleaseLock(&prlock); \
	})
	#define indent() (++numtabs[tid])
	#define unindent() (--numtabs[tid])
#endif

// use stdin, stdout & stderr even after app has closed them
void saveio();
void fixio();

// print error and exit()
void die(string msg) __attribute__((noreturn));

#endif