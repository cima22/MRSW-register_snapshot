# the compiler: gcc for C program, g++ for C++
CC = gcc

# compiler flags:
# -g adds debugging information to the executable file
# -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall -fopenmp

# the build target executables:
TARGET1 = TestLinerizability
TARGET2 = TestPSnapshot

all: $(TARGET1) $(TARGET2)

$(TARGET1): mrmwREG.o PSnapshot.o TestLinerizability.o
	$(CC) $(CFLAGS) -o $(TARGET1) mrmwREG.o PSnapshot.o TestLinerizability.o

$(TARGET2): mrmwREG.o PSnapshot.o TestPSnapshot.o
	$(CC) $(CFLAGS) -o $(TARGET2) mrmwREG.o PSnapshot.o TestPSnapshot.o

mrmwREG.o: mrmwREG.c mrmwREG.h
	$(CC) $(CFLAGS) -c mrmwREG.c

PSnapshot.o: PSnapshot.c PSnapshot.h
	$(CC) $(CFLAGS) -c PSnapshot.c

TestLinerizability.o: TestLinerizability.c mrmwREG.h PSnapshot.h
	$(CC) $(CFLAGS) -c TestLinerizability.c

TestPSnapshot.o: TestPSnapshot.c mrmwREG.h PSnapshot.h
	$(CC) $(CFLAGS) -c TestPSnapshot.c

clean:
	$(RM) $(TARGET1) $(TARGET2) *.o *~
