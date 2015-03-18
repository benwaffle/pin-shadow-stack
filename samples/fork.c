#include <stdio.h>
#include <unistd.h>

int main() {
    if (fork())
        puts("parent");
    else
        puts("child");
}
