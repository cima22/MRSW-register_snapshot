#include "mrswRegisterOpenMP.h"

void createAtomicMRSWRegister(AtomicMRSWRegister* reg, int init,  int readers) {
    reg->a_table = (AtomicSRSWRegister**) calloc(readers, sizeof(AtomicSRSWRegister*));
    for(int i = 0;i<readers;i++){
        reg->a_table[i] = (AtomicSRSWRegister*) calloc(readers, sizeof(AtomicSRSWRegister));
        for(int j = 0;j<readers;j++) {
            createAtomicSRSWRegister(&(reg->a_table[i][j]),init);
        }
    }
    reg->capacity = readers;
}


StampedValue readMRSW(AtomicMRSWRegister* reg) {
    int me = omp_get_thread_num();
    StampedValue value = readSRSW(&(reg->a_table[me][me]));
    for (int i = 0; i < reg->capacity; ++i) {
        StampedValue columnEntry = readSRSW(&(reg->a_table[i][me]));
        value = value.stamp >= columnEntry.stamp ? value : columnEntry;
    }
    for (int i = 0; i < reg->capacity; ++i) {
        writeSRSW(&(reg->a_table[me][i]),value);
    }
    return value;
}

void writeMRSW(AtomicMRSWRegister* reg, long* threadLastStamp,int v) {
    long stamp = *threadLastStamp + 1;
    *threadLastStamp = stamp;
    for (int i = 0; i < reg->capacity; ++i) {
        writeSRSW(&(reg->a_table[i][i]),(StampedValue){*threadLastStamp,v});
    }
}

int CopyContentMRSWRegister(AtomicMRSWRegister* x, AtomicMRSWRegister* y){
    createAtomicMRSWRegister(x,0,y->capacity);
    for (int i = 0; i < x->capacity; ++i) {
        for (int j = 0; j < x->capacity; ++j) {
            if(CopyContentSRSWRegister(&(x->a_table[i][j]),&(y->a_table[i][j])) == EXIT_FAILURE){
                return EXIT_FAILURE;
            }
        }
    }
    return EXIT_SUCCESS;
}

void freeAtomicMRSWRegister(AtomicMRSWRegister* reg){
    for(int i = 0;i<reg->capacity;i++){
        free(reg->a_table[i]);
    }
    free(reg->a_table);
}
