CC=gcc
CFLAGS=-Wall -Wextra -std=c11

all: SRSW-AtomicRegister run

SRSW-AtomicRegister: SRSW-AtomicRegister.c
	$(CC) $(CFLAGS) -o SRSW-AtomicRegister SRSW-AtomicRegister.c

run:
	./SRSW-AtomicRegister
clean:
	rm SRSW-AtomicRegister
