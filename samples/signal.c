#include <signal.h>

extern char *strsignal(int); // weird issue with simply including string.h

void handle(int sig) {
	printf("handling signal '%s'\n", strsignal(sig));
}

int main() {
	printf("handler @ %p\n", &handle);
	signal(SIGUSR1, handle);
	raise(SIGUSR1);
}
