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
    long* lastStamp;
    AtomicSRSWRegister*** a_table;
    int sizeOfTable;
} AtomicMRSWRegister;

void createAtomicMRSWRegister(AtomicMRSWRegister* reg, void* init,  int readers);
void* readMRSW(AtomicMRSWRegister* reg);
void writeMRSW(AtomicMRSWRegister* reg, void* v);
void destroyAtomicMRSWRegister(AtomicMRSWRegister* reg);

#endif  // MRSW_REGISTER_H
