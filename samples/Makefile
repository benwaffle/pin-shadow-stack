CC=clang
CFLAGS=-fomit-frame-pointer -g
all: hello longjmp threads gtk

threads:
	clang $(CFLAGS) -lpthread threads.c -o threads

gtk:
	clang gtk.c $(CFLAGS) $(shell pkg-config --cflags --libs gtk+-3.0) -o gtk

clean:
	rm hello longjmp threads gtk

