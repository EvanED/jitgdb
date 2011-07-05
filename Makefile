.PHONY: all
all: libtest.so test

libtest.so: libtest.cc
	g++ -o libtest.so -shared libtest.cc -W -Wall -g -fPIC

test: test.cc
	g++ -o test test.cc -g
