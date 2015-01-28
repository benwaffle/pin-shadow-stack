CC=clang
CFLAGS=-g
all: hello longjmp threads gtk optimized

threads:
	gcc $(CFLAGS) -std=c11 -lpthread $@.c -o $@

gtk:
	gcc $(CFLAGS) $(shell pkg-config --cflags --libs gtk+-3.0) $@.c -o $@

optimized:
	gcc $(CFLAGS) -O3 $@.c -o $@

.PHONY: clean
clean:
	rm hello longjmp threads gtk optimized

