//
// Created by AK Gunter on 10.06.23.
//

#include <stdatomic.h>

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

int getValue(mrmwREG reg);

int getPid(mrmwREG reg);

int getSn(mrmwREG reg);


