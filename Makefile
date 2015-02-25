DEBUG=0
CXX=clang++
PIN_ROOT=../pin-2.14

ifeq ($(DEBUG),1)
	TOOL_OBJ = pintool_debug
else
	TOOL_OBJ = pintool_no_debug
endif

include makefile.intel

TOOL_CXXFLAGS += -std=c++11 -g

ifeq ($(DEBUG),1)
	TOOL_CXXFLAGS += -DDEBUG
endif

run: demos
	$(PIN_ROOT)/pin.sh -t obj-intel64/shadow_stack.so -- ./samples/hello

demos:
	make -sC samples
