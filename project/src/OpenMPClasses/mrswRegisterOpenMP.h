#ifndef MRSW_REGISTER_H
#define MRSW_REGISTER_H

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include "stampedValue.h"
#include "srswRegisterOpenMP.h"
#include <omp.h>

typedef struct {
    AtomicSRSWRegister** a_table;
    int capacity;
} AtomicMRSWRegister;

void createAtomicMRSWRegister(AtomicMRSWRegister* reg, int init,  int readers);
StampedValue readMRSW(AtomicMRSWRegister* reg);
void writeMRSW(AtomicMRSWRegister* reg, long *ThreadLastStamp,int v);
int CopyContentMRSWRegister(AtomicMRSWRegister* x, AtomicMRSWRegister* y);
int MaxMRSW(AtomicMRSWRegister* reg, AtomicSRSWRegister* returnedReg);
void freeAtomicMRSWRegister(AtomicMRSWRegister* reg);

#endif  // MRSW_REGISTER_H