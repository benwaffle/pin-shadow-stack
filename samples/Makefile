CC=clang
CFLAGS=-g -std=c11 -include stdio.h
CXXFLAGS=-g -std=c++11
BINS=hello longjmp longjmp2 threads gtk optimized tailcall c++ c++2 thunk signal fork dl

all: $(BINS)

threads:   LDFLAGS += -lpthread
dl:        LDFLAGS += -ldl
gtk:       CFLAGS += $(shell pkg-config --cflags --libs gtk+-3.0)
optimized: CFLAGS += -O3
tailcall:  CFLAGS += -O3 -fno-inline -include stdlib.h
thunk:     CC = gcc
thunk:     CFLAGS += -std=gnu11

.PHONY: clean
clean:
	rm $(BINS)

