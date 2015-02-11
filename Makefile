DEBUG=1
CXX=clang++
PIN_ROOT=../pin-2.14

include makefile.intel

TOOL_CXXFLAGS += -std=c++11 -g

ifeq ($(DEBUG),1)
	TOOL_CXXFLAGS += -DDEBUG
endif

run: demos
	$(PIN_ROOT)/pin.sh -t obj-intel64/shadow-stack.so -- ./samples/hello

demos:
	make -sC samples
