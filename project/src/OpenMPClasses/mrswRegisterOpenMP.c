// #include <stdlib.h>
// #include <pthread.h>
// #include <stdio.h>
// #include <sys/syscall.h>
// #include "stampedValue.h"
// #include "srswRegisterOpenMP.c"
// #include <omp.h>
// typedef struct {
//     long* lastStamp;
//     AtomicSRSWRegister*** a_table;
//     int sizeOfTable
// } AtomicMRSWRegister;
#include "mrswRegisterOpenMP.h"


void createAtomicMRSWRegister(AtomicMRSWRegister* reg, void* init,  int readers) {
    // the register has to have the memory allocated before entering the function
    // AtomicMRSWRegister* reg = (AtomicMRSWRegister*) malloc(sizeof(AtomicMRSWRegister));
    reg->a_table = (AtomicSRSWRegister***) malloc(readers * sizeof(AtomicSRSWRegister**));
    for(int i = 0;i<readers;i++){
        reg->a_table[i] = (AtomicSRSWRegister**) malloc(readers * sizeof(AtomicSRSWRegister*));
        for(int j = 0;j<readers;j++) {
            reg->a_table[i][j] = (AtomicSRSWRegister*)malloc(sizeof(AtomicSRSWRegister));
            createAtomicSRSWRegister(reg->a_table[i][j],init);
        }
    }
    reg->sizeOfTable = readers;
    int numThreads = omp_get_num_threads();
    reg->lastStamp = (long*)malloc(numThreads * sizeof(long));
    for(int i = 0;i<numThreads;i++) {
        reg->lastStamp[i] = 0;
    }

    // return reg;
}

void* readMRSW(AtomicMRSWRegister* reg) {
    int  me = omp_get_thread_num();
    AtomicSRSWRegister* value = reg->a_table[me][me];
    int sizeOfTable = reg->sizeOfTable;
    for (int i = 0; i < (int) sizeOfTable; i++) {
        if(isFirstBigger(value->r_value, reg->a_table[i][me]->r_value))
            copyAtomicSRSWRegisterOpenMP(value, reg->a_table[i][me]);
    }
    for (int i = 0; i < sizeOfTable; i++) {
        reg->a_table[me][i] = value;
    }
    // what should we return, cause returning the SRSW register is kinda crazy; therefore will return the value of the best SRSW register
    // so I repeat, rn I am not returning anything thread-specific, only the register's value
    return value->r_value->value;
}

void writeMRSW(AtomicMRSWRegister* reg, void* v){
    int threadNum = omp_get_thread_num();
    long stamp = reg->lastStamp[threadNum];

    AtomicSRSWRegister* regSRSW = (AtomicSRSWRegister*)malloc(sizeof(AtomicSRSWRegister));
    createAtomicSRSWRegister(regSRSW,v);

    reg->lastStamp[threadNum] = stamp;

    int sizeOfTable = reg->sizeOfTable;
    for (int i = 0; i < sizeOfTable; i++) {
        // destroyAtomicSRSWRegister(reg->a_table[i][i]);
        reg->a_table[i][i] = regSRSW;
    }
}

// void destroyAtomicMRSWRegister(AtomicMRSWRegister* reg){
//     for(int i = 0;i<reg->sizeOfTable;i++) {
//         for(int j = 0;j<reg->sizeOfTable;j++) {
//             destroyAtomicSRSWRegister(reg->a_table[i][j]);
//         }
//         free(reg->a_table[i]);
//     }
//     free(reg->a_table);
// }