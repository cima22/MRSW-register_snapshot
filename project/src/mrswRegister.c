#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "stampedValue.h"
#include "srswRegister.c"
typedef struct {
    pthread_key_t lastStamp;
    AtomicSRSWRegister*** a_table;
    int sizeOfTable
} AtomicMRSWRegister;

void initThreadSpecificKeys(AtomicMRSWRegister* reg) {
    pthread_key_create(&reg->lastStamp, NULL);
}

AtomicMRSWRegister* createAtomicMRSWRegister(void* init,  int readers) {
    AtomicMRSWRegister* reg = (AtomicMRSWRegister*) malloc(sizeof(AtomicMRSWRegister));
    reg->a_table = (AtomicSRSWRegister***) malloc(readers * sizeof(AtomicSRSWRegister**));
    for(int i = 0;i<readers;i++){
        reg->a_table[i] = (AtomicSRSWRegister**) malloc(readers * sizeof(AtomicSRSWRegister*));
        for(int j = 0;j<readers;j++) {
            reg->a_table[i][j] = createAtomicSRSWRegister(init);
        }
    }
    reg->sizeOfTable = readers;
    pthread_key_create(&reg->lastStamp, destroyThreadSpecificKeys);

    return reg;
}

void* read(AtomicMRSWRegister* reg) {
    pid_t  me = syscall(SYS_gettid);
    AtomicSRSWRegister* value = reg->a_table[me][me];
    int sizeOfTable = reg->sizeOfTable;
    for (int i = 0; i < (int) sizeOfTable; i++) {
        value = maxSRSWAtomic(value, reg->a_table[i][me]);
    }
    for (int i = 0; i < sizeOfTable; i++) {
        reg->a_table[me][i] = value;
    }
    return value->r_value->value;
}

void write(AtomicMRSWRegister* reg, void* v){
    long stamp = *((long*)pthread_getspecific(reg->lastStamp)) + 1;
    pthread_setspecific(reg->lastStamp, &stamp);
    AtomicSRSWRegister* value = createAtomicSRSWRegister(v);

    int sizeOfTable = reg->sizeOfTable;
    for (int i = 0; i < sizeOfTable; i++) {
        destroyAtomicSRSWRegister(reg->a_table[i][i]);
        reg->a_table[i][i] = value;
    }
}

void destroyAtomicMRSWRegister(AtomicMRSWRegister* reg){
    for(int i = 0;i<reg->sizeOfTable;i++) {
        for(int j = 0;j<reg->sizeOfTable;j++) {
            destroyAtomicSRSWRegister(reg->a_table[i][j]);
        }
        free(reg->a_table[i]);
    }
    free(reg->a_table);
}