#ifndef UTIL_C
#define UTIL_C

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

#define RED "\033[01;41m"
#define GREEN "\033[42m"
#define BLUE "\033[44m"
#define RESET "\033[0m"

#define insert_call(prefix, item, func, ...) \
	prefix ## _InsertCall(item, IPOINT_BEFORE, (AFUNPTR)func, __VA_ARGS__, IARG_END)

#define PIN "\033[32m[PINTOOL]\033[0m "

int stdin_copy = -1,
    stdout_copy = -1,
    stderr_copy = -1;

#pragma GCC diagnostic ignored "-Wunused"
static void saveio(){
    stdin_copy = dup(STDIN_FILENO);
    stdout_copy = dup(STDOUT_FILENO);
    stderr_copy = dup(STDERR_FILENO);
}

static void fixio(){
   dup2(stdin_copy, STDIN_FILENO);
   dup2(stdout_copy, STDOUT_FILENO);
   dup2(stderr_copy, STDERR_FILENO);
}

#endif
