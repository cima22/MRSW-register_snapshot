#ifndef SRSW_REGISTER_OPENMP_H
#define SRSW_REGISTER_OPENMP_H

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include "stampedValue.h"
#include <omp.h>
#include <string.h>
#include <unistd.h>

typedef struct {
    long stamp;
    StampedValue r_value;
} StampedStampedValue;

typedef struct {
    long lastStamp;
    StampedStampedValue lastRead;
    StampedStampedValue r_value;
} AtomicSRSWRegister;

void createAtomicSRSWRegister(AtomicSRSWRegister* reg, int init);
StampedValue readSRSW(AtomicSRSWRegister* reg);
void writeSRSW(AtomicSRSWRegister* reg, StampedValue v);
int CopyContentSRSWRegister(AtomicSRSWRegister* x, AtomicSRSWRegister* y);
int max(StampedStampedValue* x, StampedStampedValue* y, StampedStampedValue* result);
#endif  // SRSW_REGISTER_OPENMP_H