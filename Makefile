CC=gcc
CXX=g++
CFLAGS=-Wall -Wextra -std=c11

.PHONY: all clean run

all: SRSW-AtomicRegister ThreadTestingRegular

SRSW-AtomicRegister: SRSW-AtomicRegister.c
	$(CC) $(CFLAGS) -o $@ $<

ThreadTestingRegular: ThreadTestingRegular.cpp
	$(CXX) -o $@ $<

run: SRSW-AtomicRegister ThreadTestingRegular
	./SRSW-AtomicRegister
	./ThreadTestingRegular

clean:
	rm -f SRSW-AtomicRegister ThreadTestingRegular