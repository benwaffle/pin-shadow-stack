#include <cstdio>
#include <unistd.h>
#include "util.h"

int indent = 0;

void pr_indent() {
    for (int i = 0; i < indent; ++i)
        printf("\t"); 
}

int stdin_copy = -1,
    stdout_copy = -1,
    stderr_copy = -1;

// #pragma GCC diagnostic ignored "-Wunused"
//static
void saveio() {
    stdin_copy = dup(STDIN_FILENO);
    stdout_copy = dup(STDOUT_FILENO);
    stderr_copy = dup(STDERR_FILENO);
}

// static
void fixio() {
    dup2(stdin_copy, STDIN_FILENO);
    dup2(stdout_copy, STDOUT_FILENO);
    dup2(stderr_copy, STDERR_FILENO);
}

void die(string msg) {
    fprintf(stderr, RED "%s\n" RESET, msg.c_str());
    exit(1);
}