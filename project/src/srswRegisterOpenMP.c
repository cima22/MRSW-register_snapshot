#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include "stampedValue.h"
#include <omp.h>

// made an array of stampedValues, one for each thread (can be modified later)
// this was the easier approach I thought of
typedef struct {
    long* lastStamp;
    StampedValue** lastRead;
    StampedValue* r_value;
} AtomicSRSWRegister;

// Create a new AtomicSRSWRegister object
AtomicSRSWRegister* createAtomicSRSWRegister(void* init) {

    AtomicSRSWRegister* reg = (AtomicSRSWRegister*)malloc(sizeof(AtomicSRSWRegister));

    reg->r_value = calloc(1, sizeof(StampedValue));
    if(initStampedValue(reg->r_value, init) !=0){
        fprintf(stderr, "Stamped value init failed");
        return NULL;
    }

    int num_threads = omp_get_num_threads();
    reg->lastRead = (StampedValue**)malloc(num_threads * sizeof(StampedValue*));
    reg->lastStamp = (long*)malloc(num_threads * sizeof(long));
    
    for(int i = 0;i<num_threads;i++){
        reg->lastRead[i] = calloc(1, sizeof(StampedValue));
        initStampedValue(reg->lastRead[i], init);
        reg->lastStamp[i] = 0;
    }
    return reg;
}

// possible memory leaks here; don't know which variables should be copies(with malloc) and which should be just copy; don't care currently
// allocate as much as we want
void* read(AtomicSRSWRegister* reg) {
    StampedValue* value = (StampedValue*)malloc(sizeof(StampedValue));
    createStampedValue(value, reg->r_value->stamp,reg->r_value->value);
    int threadId = omp_get_thread_num();
    StampedValue* last = (StampedValue*)malloc(sizeof(StampedValue));
    createStampedValue(last, reg->lastRead[threadId]->stamp,reg->lastRead[threadId]->value);
    StampedValue* result = (StampedValue*)malloc(sizeof(StampedValue));
    max(value, last);
    result->stamp = value->stamp;
    result->value = value->value;
    reg->lastRead[threadId] = result;
    return result->value;
}

void write(AtomicSRSWRegister* reg, void* v) {
    int threadId = omp_get_thread_num();
    long stamp = reg->lastStamp[threadId] + 1;
    createStampedValue(reg->r_value, stamp, v);
    reg->lastStamp[threadId] = stamp;
}