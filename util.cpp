#include <cstdio>
#include <cstdarg>
#include <unistd.h>
#include <fcntl.h>
#include "pin.H"
#include "util.h"

PIN_LOCK prlock;

void locked(std::function<void(THREADID)> func){
    int tid = PIN_ThreadId();
    PIN_GetLock(&prlock, tid);
    func(tid);
    PIN_ReleaseLock(&prlock);
}

#ifdef DEBUG
    int numtabs[128] = {0};

    void pr_indent(int tid) {
        for (int i = 0; i < numtabs[tid]; ++i)
            putchar('\t');
    }

    int lockprf(const char *fmt, ...) {
        // if (fcntl(1, F_GETFL) == -1 && errno == EBADF) fixio();

        va_list args;
        va_start(args, fmt);
        PIN_GetLock(&prlock, PIN_ThreadId());
        pr_indent(PIN_ThreadId());
        int ret = vprintf(fmt, args);
        PIN_ReleaseLock(&prlock);
        va_end(args);
        return ret;
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
