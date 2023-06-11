# the compiler: gcc for C program, g++ for C++
CC = gcc

# compiler flags:
# -g adds debugging information to the executable file
# -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall -fopenmp

# the build target executable:
TARGET = TestUpdate

all: $(TARGET)

$(TARGET): mrmwREG.o PSnapshot.o TestUpdate.o
	$(CC) $(CFLAGS) -o $(TARGET) mrmwREG.o PSnapshot.o TestUpdate.o

mrmwREG.o: mrmwREG.c mrmwREG.h
	$(CC) $(CFLAGS) -c mrmwREG.c

PSnapshot.o: PSnapshot.c PSnapshot.h
	$(CC) $(CFLAGS) -c PSnapshot.c

TestUpdate.o: TestUpdate.c mrmwREG.h PSnapshot.h
	$(CC) $(CFLAGS) -c TestUpdate.c

clean:
	$(RM) $(TARGET) *.o *~
