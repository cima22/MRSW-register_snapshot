//
// Created by AK Gunter on 10.06.23.
//

#ifndef MRSW_REGISTER_SNAPSHOT_MRSWPSNAP_H
#define MRSW_REGISTER_SNAPSHOT_MRSWPSNAP_H


#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/syscall.h>
#include "srswPSnap.h"
#include "pSnapSV.h"
#include <omp.h>

typedef struct {
    AtomicSRSWRegister** a_table;
    int sizeOfTable;
} AtomicMRSWRegister;

void createAtomicMRSWRegister(AtomicMRSWRegister* reg, int* init,  int readers);
int* readMRSW(AtomicMRSWRegister* reg, long ThreadLastStamp);
void writeMRSW(AtomicMRSWRegister* reg, long ThreadLastStamp,int* v);
void CopyContentMRSWRegister(AtomicMRSWRegister* x, AtomicMRSWRegister* y);
int MaxMRSW(AtomicMRSWRegister* reg, AtomicSRSWRegister* returnedReg);

#endif //MRSW_REGISTER_SNAPSHOT_MRSWPSNAP_H
