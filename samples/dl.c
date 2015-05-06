#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#define check(x) if (!x) { puts(dlerror()); abort(); }

int main() {
    void *h = NULL;
    double (*sin)(double) = NULL;
    
    h = dlopen("libm.so", RTLD_LAZY);
    check(h);
    sin = dlsym(h, "sin");
    check(sin);
    printf("%g\n", sin(2*3.14));
    dlclose(h);

    h = dlopen("libm.so", RTLD_NOW);
    check(h);
    sin = dlsym(h, "sin");
    check(sin);
    printf("%g\n", sin(2*3.14));
    dlclose(h);
}
