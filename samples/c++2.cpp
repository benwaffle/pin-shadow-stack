#include <cstdio>

int main() {
    try {
        throw 123;
    } catch (...) {
        puts("caught");
    }

    return 0;
}
