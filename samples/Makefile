CC=clang
CFLAGS=-g -std=c11 -include stdio.h
BINS=hello longjmp longjmp2 threads gtk optimized tailcall

all: $(BINS)

threads:   LDFLAGS += -lpthread
gtk:       CFLAGS += $(shell pkg-config --cflags --libs gtk+-3.0)
optimized: CFLAGS += -O3
tailcall:  CFLAGS += -O3 -fno-inline -include stdlib.h

.PHONY: clean
clean:
	rm $(BINS)

