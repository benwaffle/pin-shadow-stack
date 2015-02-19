#include <stdio.h>

__attribute__((noinline)) int get_rand(){
	return 4;
}

int do_stuff(){
	printf("doing stuff here\n");
	return get_rand();
}

int main() {
	int x = do_stuff();
	printf("%d\n", x);
}
