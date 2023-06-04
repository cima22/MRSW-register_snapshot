# compiler
CC = gcc

# compiler flags
CFLAGS  = -g -Wall -fopenmp

# the build target executables
TARGETSRSW = TestsrswRegisterOpenMP
TARGETMRSW = TestmrswRegisterOpenMP
TARGETWFS = TestWFSnapShot

all: $(TARGETSRSW) $(TARGETMRSW) $(TARGETWFS)

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

WFSnapshot.o: WFSnapshot.c WFSnapshot.h srswRegisterOpenMP.o mrswRegisterOpenMP.o
	$(CC) $(CFLAGS) -c WFSnapshot.c

$(TARGETWFS): TestWFSnapShot.c WFSnapshot.o srswRegisterOpenMP.o mrswRegisterOpenMP.o stampedValue.o
	$(CC) $(CFLAGS) -o $(TARGETWFS) TestWFSnapShot.c WFSnapshot.o srswRegisterOpenMP.o mrswRegisterOpenMP.o stampedValue.o

clean:
	$(RM) $(TARGETSRSW) $(TARGETMRSW) $(TARGETWFS) *.o *~
