#pragma once

#include <string>
#include <iostream>
#include <functional>
#include <cassert>

// terminal colors
#define RED    "\e[41m"
#define GREEN  "\e[32m"
#define YELLOW "\e[33m"
#define BLUE   "\e[44m"
#define RESET  "\e[0m"

#define likely(x) __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)

// logging

#ifdef DEBUG
	extern int numtabs[128];

	void pr_indent(int tid);
	#define indent() (++numtabs[PIN_ThreadId()])
	#define unindent() (--numtabs[PIN_ThreadId()])

	int lockprf (const char *fmt, ...) __attribute__(( format(printf, 1, 2) ));
#endif

extern PIN_LOCK prlock;
// TODO: avoid cluttered output from multiple threads (print to file?)
// TODO: no lock on single thread
void locked(std::function<void(THREADID)> func);

// use stdin, stdout & stderr even after app has closed them
void saveio();
void fixio();

// print error and exit()
void die (std::string msg) __attribute__(( noreturn ));
