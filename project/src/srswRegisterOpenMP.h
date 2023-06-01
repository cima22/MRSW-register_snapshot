#ifndef ATOMIC_SRSW_REGISTER_H
#define ATOMIC_SRSW_REGISTER_H

#include "stampedValue.h"

typedef struct {
    long* lastStamp;
    StampedValue** lastRead;
    StampedValue* r_value;
} AtomicSRSWRegister;

AtomicSRSWRegister* createAtomicSRSWRegister(void* init);
void* read(AtomicSRSWRegister* reg);
void write(AtomicSRSWRegister* reg, void* v);
// void destroyAtomicSRSWRegister(AtomicSRSWRegister* reg);

#endif /* ATOMIC_SRSW_REGISTER_H */
