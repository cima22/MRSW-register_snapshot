#include "srswRegisterOpenMP.h"

// Create a new AtomicSRSWRegister object
void createAtomicSRSWRegister(AtomicSRSWRegister* reg,int init) {
    reg->r_value.stamp = 0;
    reg->r_value.r_value.stamp = 0;
    reg->r_value.r_value.value = init;
    reg->lastRead = reg->r_value;
    reg->lastStamp = 0;
}

int max(StampedStampedValue* x, StampedStampedValue* y, StampedStampedValue* result){
    if(x == NULL || y == NULL || result == NULL){
        fprintf(stderr,"One of the parameters in max() is NULL!");
        return EXIT_FAILURE;
    }
    *result = x->stamp >= y->stamp ? *x : *y;
    return EXIT_SUCCESS;
}

// the reading takes as parameter the register and the thread that is reading from that register
StampedValue readSRSW(AtomicSRSWRegister* reg) {
    StampedStampedValue result;
    max(&reg->r_value,&reg->lastRead,&result);
    reg->lastRead = result;
    return result.r_value;
}

// The writer takes the register and the thread that writes to it
void writeSRSW(AtomicSRSWRegister* reg, StampedValue v) {
    long stamp = reg->lastStamp + 1;
    reg->r_value.r_value = v;
    reg->r_value.stamp = stamp;
    reg->lastStamp = stamp;
}

int CopyContentSRSWRegister(AtomicSRSWRegister* x, AtomicSRSWRegister* y){
    if(x == NULL || y == NULL){
        fprintf(stderr,"x or y is NULL in CopyContentSRSWRegister()");
        return EXIT_FAILURE;
    }
    *x = *y;
    return EXIT_SUCCESS;
}
