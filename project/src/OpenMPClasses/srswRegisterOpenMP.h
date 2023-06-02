#ifndef SRSW_REGISTER_OPENMP_H
#define SRSW_REGISTER_OPENMP_H

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include "stampedValue.h"
#include <omp.h>
#include <string.h>

typedef struct {
    long* lastStamp;
    StampedValue** lastRead;
    StampedValue* r_value;
} AtomicSRSWRegister;

void createAtomicSRSWRegister(AtomicSRSWRegister* reg,void* init);
void* readSRSW(AtomicSRSWRegister* reg);
void writeSRSW(AtomicSRSWRegister* reg, void* v);
int copyAtomicSRSWRegisterOpenMP(AtomicSRSWRegister* dest, const AtomicSRSWRegister* src);

#endif  // SRSW_REGISTER_OPENMP_H
