//
// Created by AK Gunter on 10.06.23.
//
#ifndef MRMWREG_H
#define MRMWREG_H
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>
#include<omp.h>

typedef struct {
    atomic_int value;
    atomic_int pid;
    atomic_int sn;
} mrmwREG;

void initREG(mrmwREG * reg, int value, int pid, int sn);

void updateREG(mrmwREG * reg, int value, int sn);

/**
 * Function that compares two registers
 *
 * @param aa teh first register
 * @param bb the second register
 * @return true if they are equal for all their attributes, else false
 */
bool compareREG(mrmwREG aa, mrmwREG bb);

void copyREG(mrmwREG*aa,mrmwREG* bb);

int getValue(mrmwREG reg);

int getPid(mrmwREG reg);

int getSn(mrmwREG reg);

void CopyAtomicInttoInt(int* vec1, atomic_int* vec2, int numOfElem);

void CopyAtomicRegisters(atomic_int* destination, atomic_int* source, int numOfElem);

void CopyInttoAtomic(atomic_int* vec1, int* vec2, int numOfElem);
#endif // MRMWREG_H