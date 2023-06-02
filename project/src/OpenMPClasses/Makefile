# compiler
CC = gcc

# compiler flags
CFLAGS  = -g -Wall -fopenmp

# the build target executables
TARGETSRSW = TestsrswRegisterOpenMP
TARGETMRSW = TestmrswRegisterOpenMP

all: $(TARGETSRSW) $(TARGETMRSW)

stampedValue.o: stampedValue.c stampedValue.h
	$(CC) $(CFLAGS) -c stampedValue.c

srswRegisterOpenMP.o: srswRegisterOpenMP.c srswRegisterOpenMP.h stampedValue.o
	$(CC) $(CFLAGS) -c srswRegisterOpenMP.c

$(TARGETSRSW): TestsrswRegisterOpenMP.c srswRegisterOpenMP.o stampedValue.o
	$(CC) $(CFLAGS) -o $(TARGETSRSW) TestsrswRegisterOpenMP.c srswRegisterOpenMP.o stampedValue.o

mrswRegisterOpenMP.o: mrswRegisterOpenMP.c mrswRegisterOpenMP.h srswRegisterOpenMP.o
	$(CC) $(CFLAGS) -c mrswRegisterOpenMP.c

$(TARGETMRSW): TestmrswRegisterOpenMP.c mrswRegisterOpenMP.o srswRegisterOpenMP.o stampedValue.o
	$(CC) $(CFLAGS) -o $(TARGETMRSW) TestmrswRegisterOpenMP.c mrswRegisterOpenMP.o srswRegisterOpenMP.o stampedValue.o

clean:
	$(RM) $(TARGETSRSW) $(TARGETMRSW) *.o *~
