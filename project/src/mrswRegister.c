#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "stampedValue.h"
typedef struct {
    pthread_key_t lastStamp;
    StampedValue*** a_table;
    int sizeOfTable
} AtomicMRSWRegister;

void initThreadSpecificKeys(AtomicMRSWRegister* reg) {
    pthread_key_create(&reg->lastStamp, NULL);
}

void destroyThreadSpecificKeys(void* data) {
    free(data);
}

AtomicMRSWRegister* createAtomicMRSWRegister(void* init,  int readers) {
    AtomicMRSWRegister* reg = (AtomicMRSWRegister*) malloc(sizeof(AtomicMRSWRegister));
    reg->a_table = (StampedValue***) malloc(readers * sizeof(StampedValue**));
    for(int i = 0;i<readers;i++){
        reg->a_table[i] = (StampedValue**) malloc(readers * sizeof(StampedValue*));
        for(int j = 0;j<readers;j++) {
            reg->a_table[i][j] = (StampedValue*) calloc(1, sizeof(StampedValue));
            if(initStampedValue(reg->a_table[i][j], init) !=0){
                fprintf(stderr, "Stamped value init failed");
    }
        }
    }
    reg->sizeOfTable = readers;
    pthread_key_create(&reg->lastStamp, destroyThreadSpecificKeys);

    return reg;
}

void* read(AtomicMRSWRegister* reg) {
    pid_t  me = syscall(SYS_gettid);
    StampedValue* value = reg->a_table[me][me];
    int sizeOfTable = reg->sizeOfTable;
    for (int i = 0; i < (int) sizeOfTable; i++) {
        value = max(value, &(reg->a_table[i][me]));
    }
    for (int i = 0; i < sizeOfTable; i++) {
        reg->a_table[me][i] = value;
    }
    return value->value;
}

void write(AtomicMRSWRegister* reg, void* v){
    long stamp = *((long*)pthread_getspecific(reg->lastStamp)) + 1;
    pthread_setspecific(reg->lastStamp, &stamp);
    StampedValue* value = calloc(1, sizeof(StampedValue));
        if(value == NULL) {
        fprintf(stderr, "Memory allocation failed");
        return;
    }
    if(createStampedValue(value, stamp, v)==EXIT_FAILURE){
        fprintf(stderr, "Stamped value creation failed");
        return;
    }
    int sizeOfTable = reg->sizeOfTable;
    for (int i = 0; i < sizeOfTable; i++) {
        reg->a_table[i][i] = value;
    }
}