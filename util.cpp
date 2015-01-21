#include <cstdio>
#include <unistd.h>
#include "pin.H"
#include "util.h"

#ifdef DEBUG
    PIN_LOCK prlock;
    int numtabs[N_MAX_THREADS] = {0};

    void pr_indent(int tid) {
        for (int i = 0; i < numtabs[tid]; ++i)
            putchar('\t');
    }
#endif

int stdin_copy = -1,
    stdout_copy = -1,
    stderr_copy = -1;

void saveio() {
    stdin_copy = dup(STDIN_FILENO);
    stdout_copy = dup(STDOUT_FILENO);
    stderr_copy = dup(STDERR_FILENO);
}

void fixio() {
    dup2(stdin_copy, STDIN_FILENO);
    dup2(stdout_copy, STDOUT_FILENO);
    dup2(stderr_copy, STDERR_FILENO);
}

void die(string msg) {
    fprintf(stderr, RED "%s" RESET "\n", msg.c_str());
    exit(1);
}