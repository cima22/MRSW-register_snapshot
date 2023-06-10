//
// Created by AK Gunter on 10.06.23.
//

#ifndef MRSW_REGISTER_SNAPSHOT_SRSWPSNAP_H
#define MRSW_REGISTER_SNAPSHOT_SRSWPSNAP_H

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include "pSnapSV.h"
#include <omp.h>
#include <string.h>

// the actual register
typedef struct {
    pSnapSv* r_value;
} AtomicSRSWRegister;

// the thread specific data
typedef struct {
    long lastStamp;
    StampedValue* lastRead;
} ThreadSpecificSRSW;

void createAtomicSRSWRegister(AtomicSRSWRegister* reg, int* init);
int* readSRSW(AtomicSRSWRegister* reg, ThreadSpecificSRSW* curentThread);
void writeSRSW(AtomicSRSWRegister* reg, ThreadSpecificSRSW* curentThread, int* v);
void createAtomicSRSWThreadSpecific(ThreadSpecificSRSW* curentThread, StampedValue* r_value);


#endif //MRSW_REGISTER_SNAPSHOT_SRSWPSNAP_H
