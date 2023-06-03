#define MRMW_REGISTER_H

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/syscall.h>
#include "stampedValue.h"
#include "mrswRegisterOpenMP.h"
#include <omp.h>

typedef struct {
    AtomicMRSWRegister* a_table;
} AtomicMRMWRegister;

int createAtomicMRMWRegister(AtomicMRMWRegister* reg, int capacity, int init);
int readMRMW(AtomicMRMWRegister* reg, long ThreadLastStamp);
void writeMRMW(AtomicMRMWRegister* reg, long ThreadLastStamp,int v);
void freeAtomicMRMWRegister(AtomicMRMWRegister* reg);
void TestingMRMWRegisterMemoryAllocated(AtomicMRMWRegister* reg);

#endif //MRMW_REGISTER_H
