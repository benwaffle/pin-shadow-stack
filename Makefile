DEBUG=0
CXX=clang++ -std=c++11 -g
PIN_ROOT=../pin-2.14

ifeq ($(DEBUG),1)
	CXX += -DDEBUG
endif

include makefile.intel

run: demos
	$(PIN_ROOT)/pin.sh -t obj-intel64/shadow-stack.so -- ./samples/hello

demos:
	@make -sC samples
