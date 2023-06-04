#ifndef MRSW_REGISTER_H
#define MRSW_REGISTER_H

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/syscall.h>
#include "stampedValue.h"
#include "srswRegisterOpenMP.h"  // Note that this should be a header file, not a .c file
#include <omp.h>

typedef struct {
    AtomicSRSWRegister** a_table;
    int sizeOfTable;
} AtomicMRSWRegister;

void createAtomicMRSWRegister(AtomicMRSWRegister* reg, int init,  int readers);
int readMRSW(AtomicMRSWRegister* reg, long ThreadLastStamp);
void writeMRSW(AtomicMRSWRegister* reg, long ThreadLastStamp,int v);
void CopyContentMRSWRegister(AtomicMRSWRegister* x, AtomicMRSWRegister* y);
int MaxMRSW(AtomicMRSWRegister* reg, AtomicSRSWRegister* returnedReg);

#endif  // MRSW_REGISTER_H