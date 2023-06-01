# compiler
CC = gcc

# compiler flags
CFLAGS  = -g -Wall -fopenmp

# the build target executable
TARGET = main

all: $(TARGET)

$(TARGET): main.c srswRegisterOpenMP.o stampedValue.o
	$(CC) $(CFLAGS) -o $(TARGET) main.c srswRegisterOpenMP.o stampedValue.o

srswRegisterOpenMP.o: srswRegisterOpenMP.c srswRegisterOpenMP.h
	$(CC) $(CFLAGS) -c srswRegisterOpenMP.c

stampedValue.o: stampedValue.c stampedValue.h
	$(CC) $(CFLAGS) -c stampedValue.c

clean:
	$(RM) $(TARGET) *.o *~
