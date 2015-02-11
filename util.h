#pragma once

#include <string>
#include <functional>
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

	void pr_indent(int tid);
	#define indent() (++numtabs[tid])
	#define unindent() (--numtabs[tid])

	int lockprf(THREADID tid, const char *fmt, ...);
#endif

extern PIN_LOCK prlock;
// TODO: avoid cluttered output from multiple threads (print to file?)
// TODO: no lock on single thread
void locked(THREADID tid, std::function<void()> func);

// use stdin, stdout & stderr even after app has closed them
void saveio();
void fixio();

// print error and exit()
void die [[noreturn]] (string msg);// __attribute__((noreturn));