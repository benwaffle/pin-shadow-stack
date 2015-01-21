CFLAGS=-fomit-frame-pointer -O3
all: hello longjmp threads gtk

threads:
	gcc -fopenmp threads.c -o threads

gtk:
	clang gtk.c $(shell pkg-config --cflags --libs gtk+-3.0) -o gtk

clean:
	rm hello longjmp threads gtk

