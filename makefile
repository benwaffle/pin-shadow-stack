CC=clang -Ofast
CXX=clang++ -std=c++11 -Ofast # ugly
DEBUG=0
PIN_ROOT=../pin-2.14
include makefile.intel

run: tmp
	$(PIN_ROOT)/pin -t obj-intel64/demo-tool.so -- ./tmp

tmp:
	$(CC) -std=c11 -ggdb -O3 tmp.c -o tmp
