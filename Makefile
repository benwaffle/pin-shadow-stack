DEBUG=0
CC=clang -Ofast
CXX=clang++ -std=c++11 -Ofast 
PIN_ROOT=../pin-2.14

ifeq ($(DEBUG),1)
	CC += -DDEBUG
	CXX += -DDEBUG
endif

include makefile.intel

run: demos
	$(PIN_ROOT)/pin -t obj-intel64/demo-tool.so -- ./samples/hello

demos:
	make -sC samples