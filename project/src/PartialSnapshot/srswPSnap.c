//
// Created by AK Gunter on 10.06.23.
//

#include "srswPSnap.h"

// create for a thread allocate memory
void createAtomicSRSWThreadSpecific(ThreadSpecificSRSW* curentThread, StampedValue* r_value) {
    curentThread->lastStamp = 0;
    curentThread->lastRead = (StampedValue*)calloc(1, sizeof(StampedValue*));

    createStampedValue(curentThread->lastRead, r_value->stamp, r_value->value);
}

// Create a new AtomicSRSWRegister object
void createAtomicSRSWRegister(AtomicSRSWRegister* reg,int* init) {

    reg->r_value =(StampedValue*)calloc(1, sizeof(StampedValue));
    if(initStampedValue(reg->r_value, init) !=0){
        fprintf(stderr, "Stamped value init failed");
        return;
    }
}

// the reading takes as parameter the register and the thread that is reading from that register
int* readSRSW(AtomicSRSWRegister* reg, ThreadSpecificSRSW* curentThread) {
    StampedValue* value = (StampedValue*)calloc(1, sizeof(StampedValue));
    createStampedValue(value, reg->r_value->stamp,reg->r_value->value);

    StampedValue* last = (StampedValue*)calloc(1, sizeof(StampedValue));
    createStampedValue(last, curentThread->lastRead->stamp, curentThread->lastRead->value);

    StampedValue* result = (StampedValue*)calloc(1, sizeof(StampedValue));
    copyStampedValueSecondIntoFirst(value, last);

    result->stamp = value->stamp;
    result->value = value->value;

    createStampedValue(curentThread->lastRead, result->stamp, result->value);
    return result->value;
}

// The writer takes the register and the thread that writes to it
void writeSRSW(AtomicSRSWRegister* reg, ThreadSpecificSRSW* curentThread, int* v) {
    long stamp = curentThread->lastStamp + 1;
    createStampedValue(reg->r_value, stamp, v);
    curentThread->lastStamp = stamp;
}