#ifndef UTIL_C
#define UTIL_C

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

// terminal colors
#define RED 	"\e[41m"
#define GREEN 	"\e[32m"
#define YELLOW 	"\e[33m"
#define BLUE 	"\e[34m"
#define PURPLE	"\e[35m"
#define RESET 	"\e[0m"

int indent = 0;
void pr_indent() {
	for (int i = 0; i < indent; ++i)
		printf("\t"); 
}

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

[[noreturn]] void die(string msg) {
	fprintf(stderr, RED "%s\n" RESET, msg.c_str());
	exit(1);
}

#endif
