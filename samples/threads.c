#include <stdio.h>
#include <assert.h>
#include <pthread.h>

#define NTHREADS 4

void *eachthread(void *data){
	printf("thread %d\n", (int)data);
	return NULL;
}

int main() {
	pthread_t ts[NTHREADS];
	for (int i = 0; i < NTHREADS; ++i)
		pthread_create(&ts[i], NULL, &eachthread, (void*)i);
	for (int i = 0; i < NTHREADS; ++i)
		pthread_join(ts[i], NULL);
}
