// #include <stdlib.h>
// #include <pthread.h>
// #include <stdio.h>
// #include "stampedValue.h"
// #include <omp.h>

// // made an array of stampedValues, one for each thread (can be modified later)
// // this was the easier approach I thought of
// typedef struct {
//     long* lastStamp;
//     StampedValue** lastRead;
//     StampedValue* r_value;
// } AtomicSRSWRegister;
#include "srswRegisterOpenMP.h"

// Create a new AtomicSRSWRegister object
void createAtomicSRSWRegister(AtomicSRSWRegister* reg,void* init) {

    // AtomicSRSWRegister* reg = (AtomicSRSWRegister*)malloc(sizeof(AtomicSRSWRegister));

    reg->r_value = calloc(1, sizeof(StampedValue));
    if(initStampedValue(reg->r_value, init) !=0){
        fprintf(stderr, "Stamped value init failed");
        return;
    }

    int num_threads = omp_get_num_threads();
    reg->lastRead = (StampedValue**)malloc(num_threads * sizeof(StampedValue*));
    reg->lastStamp = (long*)malloc(num_threads * sizeof(long));
    
    for(int i = 0;i<num_threads;i++){
        reg->lastRead[i] = calloc(1, sizeof(StampedValue));
        initStampedValue(reg->lastRead[i], init);
        reg->lastStamp[i] = 0;
    }
}

// possible memory leaks here; don't know which variables should be copies(with malloc) and which should be just copy; don't care currently
// allocate as much as we want
void* readSRSW(AtomicSRSWRegister* reg) {
    StampedValue* value = (StampedValue*)malloc(sizeof(StampedValue));
    createStampedValue(value, reg->r_value->stamp,reg->r_value->value);
    int threadId = omp_get_thread_num();
    StampedValue* last = (StampedValue*)malloc(sizeof(StampedValue));
    createStampedValue(last, reg->lastRead[threadId]->stamp,reg->lastRead[threadId]->value);
    StampedValue* result = (StampedValue*)malloc(sizeof(StampedValue));
    copyStampedValueSecondIntoFirst(value, last);
    result->stamp = value->stamp;
    result->value = value->value;
    reg->lastRead[threadId] = result;
    return result->value;
}

void writeSRSW(AtomicSRSWRegister* reg, void* v) {
    int threadId = omp_get_thread_num();
    long stamp = reg->lastStamp[threadId] + 1;
    createStampedValue(reg->r_value, stamp, v);
    reg->lastStamp[threadId] = stamp;
}

int copyAtomicSRSWRegisterOpenMP(AtomicSRSWRegister* dest, const AtomicSRSWRegister* src) {
    if (dest == NULL || src == NULL) {
        fprintf(stderr, "Source or destination register is null.");
        return EXIT_FAILURE;
    }

    int num_threads = omp_get_num_threads();
    
    dest->lastStamp = (long*)malloc(num_threads * sizeof(long));
    if (dest->lastStamp == NULL) {
        fprintf(stderr, "Memory allocation for lastStamp failed.");
        return EXIT_FAILURE;
    }

    memcpy(dest->lastStamp, src->lastStamp, num_threads * sizeof(long));
    
    dest->lastRead = (StampedValue**)malloc(num_threads * sizeof(StampedValue*));
    if (dest->lastRead == NULL) {
        fprintf(stderr, "Memory allocation for lastRead failed.");
        free(dest->lastStamp); // release previously allocated memory
        return EXIT_FAILURE;
    }

    for(int i = 0; i < num_threads; i++){
        dest->lastRead[i] = (StampedValue*)malloc(sizeof(StampedValue));
        duplicateStampedValue(dest->lastRead[i], src->lastRead[i]);
        if (dest->lastRead[i] == NULL) {
            for(int j = 0; j < i; j++) { // free already copied StampedValues
                freeStampedValue(dest->lastRead[j]);
            }
            free(dest->lastStamp); // release previously allocated memory
            free(dest->lastRead);  // release already allocated memory
            return EXIT_FAILURE;
        }
    }
    dest->r_value = (StampedValue*)malloc(sizeof(StampedValue));
    duplicateStampedValue(dest->r_value,src->r_value);
    if (dest->r_value == NULL) {
        for(int i = 0; i < num_threads; i++) { // free already copied StampedValues
            freeStampedValue(dest->lastRead[i]);
        }
        free(dest->lastStamp); // release previously allocated memory
        free(dest->lastRead);  // release already allocated memory
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}