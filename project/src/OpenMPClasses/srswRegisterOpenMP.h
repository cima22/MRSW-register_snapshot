#ifndef SRSW_REGISTER_OPENMP_H
#define SRSW_REGISTER_OPENMP_H

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include "stampedValue.h"
#include <omp.h>
#include <string.h>

// the actual register
typedef struct {
    StampedValue* r_value;
} AtomicSRSWRegister;

// the thread specific data
typedef struct {
    long lastStamp;
    StampedValue* lastRead;
} ThreadSpecificSRSW;

void createAtomicSRSWRegister(AtomicSRSWRegister* reg, int init);
int readSRSW(AtomicSRSWRegister* reg, ThreadSpecificSRSW* curentThread);
void writeSRSW(AtomicSRSWRegister* reg, ThreadSpecificSRSW* curentThread, int v);
void createAtomicSRSWThreadSpecific(ThreadSpecificSRSW* curentThread, StampedValue* r_value);

#endif  // SRSW_REGISTER_OPENMP_H