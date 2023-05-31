#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include "stampedValue.h"

typedef struct {
    pthread_key_t lastStampKey;
    pthread_key_t lastReadKey;
    StampedValue* r_value;
} AtomicSRSWRegister;

//TODO: REWORK FOR APPROPRIATE MEMORY ALLOCATION AND CODING STYLE

// Initialize the thread-specific keys
void initThreadSpecificKeys(AtomicSRSWRegister* reg) {
    pthread_key_create(&reg->lastStampKey, NULL);
    pthread_key_create(&reg->lastReadKey, NULL);
}

// Thread-specific key destructor to free memory (TODO: REWORK FOR APPROPRIATE CLEAN)
void destroyThreadSpecificKeys(void* data) {
    free(data);
}

// Create a new AtomicSRSWRegister object
AtomicSRSWRegister* createAtomicSRSWRegister(void* init) {
    AtomicSRSWRegister* reg = (AtomicSRSWRegister*)malloc(sizeof(AtomicSRSWRegister));
    reg->r_value = calloc(1, sizeof(StampedValue));
    if(initStampedValue(reg->r_value, init) !=0){
        fprintf(stderr, "Stamped value init failed");
        return reg;
    }

    pthread_key_create(&reg->lastStampKey, destroyThreadSpecificKeys);
    pthread_key_create(&reg->lastReadKey, destroyThreadSpecificKeys);
    return reg;
}

// Read the value from the AtomicSRSWRegister
void* read(AtomicSRSWRegister* reg) {
    StampedValue* value = reg->r_value;
    StampedValue* last = pthread_getspecific(reg->lastReadKey);
    max(value, last); //TODO modify this line after max modification
    pthread_setspecific(reg->lastReadKey, value);
    return value->value;
}

// Write a new value to the AtomicSRSWRegister
void write(AtomicSRSWRegister* reg, void* v) {
    long stamp = *((long*)pthread_getspecific(reg->lastStampKey)) + 1;
    reg->r_value = calloc(1, sizeof(StampedValue));
    if(reg->r_value == NULL) {
        fprintf(stderr, "Memory allocation failed");
        return;
    }
    if(createStampedValue(reg->r_value, stamp, v)==EXIT_FAILURE){
        fprintf(stderr, "Stamped value creation failed");
        return;
    }
    pthread_setspecific(reg->lastStampKey, &stamp);
}

