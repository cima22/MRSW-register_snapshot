#include "mrswRegisterOpenMP.h"

void createAtomicMRSWRegister(AtomicMRSWRegister* reg, void* init,  int readers) {
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
    // TestingMRSWRegisterMemoryAllocated(reg);
}

// somewhat the reg doesn't have the a_table[][]->lastRead[] initialised and that is what is fucking up
void* readMRSW(AtomicMRSWRegister* reg) {
    printf("\niteratie\n");
    int  me = omp_get_thread_num();
    // printf("%d",me);
    AtomicSRSWRegister* value = reg->a_table[me][me];
    TestingMRSWRegisterMemoryAllocated(reg);
    // TestingSRSWRegisterMemoryAllocated(value);
    int sizeOfTable = reg->sizeOfTable;
    for (int i = 0; i < (int) sizeOfTable; i++) {
        if(isFirstBigger(value->r_value, reg->a_table[i][me]->r_value)){

            copyAtomicSRSWRegisterOpenMP(value, reg->a_table[i][me]);
        }
    }
    for (int i = 0; i < sizeOfTable; i++) {
        reg->a_table[me][i] = value;
    }
    // what should we return, cause returning the SRSW register is kinda crazy; therefore will return the value of the best SRSW register
    // so I repeat, rn I am not returning anything thread-specific, only the register's value
    return value->r_value->value;
}

void writeMRSW(AtomicMRSWRegister* reg, void* v){
    printf("iteratie write");
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



void TestingMRSWRegisterMemoryAllocated(AtomicMRSWRegister* reg){
    if(reg == NULL) {
        printf("\n\nError: reg is NULL\n\n");
        return;
    }

    int readers = reg->sizeOfTable;
    
    if(reg->a_table == NULL) {
        printf("\n\nError: a_table is NULL\n\n");
        return;
    }
    
    for(int i = 0; i < readers; i++) {
        if(reg->a_table[i] == NULL) {
            printf("\n\nError: a_table[%d] is NULL\n\n", i);
            continue;
        }
        
        for(int j = 0; j < readers; j++) {
            if(reg->a_table[i][j] == NULL) {
                printf("\n\nError: a_table[%d][%d] is NULL\n\n", i, j);
                continue;
            }
            TestingSRSWRegisterMemoryAllocated(reg->a_table[i][j]);
            // for(int k = 0; k < omp_get_num_threads(); k++) {
            //     if(reg->a_table[i][j]->lastRead[k] == NULL) {
            //         printf("\n\nError: a_table[%d][%d]->lastRead[%d] is NULL\n\n", i, j, k);
            //     }
            // }
        }
    }
}