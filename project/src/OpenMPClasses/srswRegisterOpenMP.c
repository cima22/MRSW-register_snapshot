#include "srswRegisterOpenMP.h"


// create for a thread allocate memory
void createAtomicSRSWThreadSpecific(ThreadSpecificSRSW* curentThread, StampedValue* r_value) {
    curentThread->lastStamp = 0;
    curentThread->lastRead = (StampedValue*)calloc(1, sizeof(StampedValue*));

    createStampedValue(curentThread->lastRead, r_value->stamp, r_value->value);
}

// Create a new AtomicSRSWRegister object
void createAtomicSRSWRegister(AtomicSRSWRegister* reg,int init) {

    reg->r_value =(StampedValue*)calloc(1, sizeof(StampedValue));
    if(initStampedValue(reg->r_value, init) !=0){
        fprintf(stderr, "Stamped value init failed");
        return;
    }
}

void CopyContentSRSWRegister(AtomicSRSWRegister* x, AtomicSRSWRegister* y) {
    // Check for null pointers
    if (x == NULL || y == NULL) {
        fprintf(stderr, "NULL pointer received in CopyContentSRSWRegister\n");
        return;
    }

    // Check if r_value is properly initialized in y
    if (y->r_value == NULL) {
        fprintf(stderr, "r_value not initialized in y\n");
        return;
    }
    
    // Copy y's r_value to x's r_value
    copyStampedValueSecondIntoFirst(x->r_value, y->r_value);
}


// the reading takes as parameter the register and the thread that is reading from that register
int readSRSW(AtomicSRSWRegister* reg, ThreadSpecificSRSW* curentThread) {

    if( reg->r_value->stamp > curentThread->lastStamp) {
        copyStampedValueSecondIntoFirst(curentThread->lastRead, reg->r_value);
        return reg->r_value->value;
    }

    return curentThread->lastRead->value;
}

// The writer takes the register and the thread that writes to it
void writeSRSW(AtomicSRSWRegister* reg, ThreadSpecificSRSW* curentThread, int v) {
    long stamp = curentThread->lastStamp + 1;
    createStampedValue(reg->r_value, stamp, v);
    curentThread->lastStamp = stamp;
}