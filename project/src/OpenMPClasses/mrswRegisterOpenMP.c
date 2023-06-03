#include "mrswRegisterOpenMP.h"

void createAtomicMRSWRegister(AtomicMRSWRegister* reg, int init,  int readers) {
    reg->a_table = (AtomicSRSWRegister***) calloc(readers, sizeof(AtomicSRSWRegister**));
    for(int i = 0;i<readers;i++){
        reg->a_table[i] = (AtomicSRSWRegister**) calloc(readers, sizeof(AtomicSRSWRegister*));
        for(int j = 0;j<readers;j++) {
            reg->a_table[i][j] = (AtomicSRSWRegister*)calloc(1, sizeof(AtomicSRSWRegister));
            createAtomicSRSWRegister(reg->a_table[i][j],init);
        }
    }
    reg->sizeOfTable = readers;
}

int readMRSW(AtomicMRSWRegister* reg, long ThreadLastStamp) {
    int me = omp_get_thread_num();

    // Here I just copied the content of the SRSW register, did not create a pointer to the exact same location
    AtomicSRSWRegister* value = (AtomicSRSWRegister*)calloc(1, sizeof(AtomicSRSWRegister));
    value->r_value = (StampedValue*)calloc(1, sizeof(StampedValue));
    createStampedValue(value->r_value,reg->a_table[me][me]->r_value->stamp, reg->a_table[me][me]->r_value->value);

    int sizeOfTable = reg->sizeOfTable;
    for (int i = 0; i < (int) sizeOfTable; i++) {
        if(isFirstBigger(value->r_value, reg->a_table[i][me]->r_value)){

            createStampedValue(value->r_value, reg->a_table[i][me]->r_value->stamp, reg->a_table[i][me]->r_value->value);
        }
    }
    for (int i = 0; i < sizeOfTable; i++) {
        createStampedValue(reg->a_table[me][i]->r_value,value->r_value->stamp, value->r_value->value);
    }
    // The book is strange, I don't understand why it says to return an object of type StampedValue,
    // but the function is of type int ???
    return value->r_value->value;
}

void writeMRSW(AtomicMRSWRegister* reg, long ThreadLastStamp,int v) {
    long stamp = ThreadLastStamp + 1;
    ThreadLastStamp = stamp;
    AtomicSRSWRegister* regSRSW = (AtomicSRSWRegister*)calloc(1, sizeof(AtomicSRSWRegister));
    createAtomicSRSWRegister(regSRSW,v);

    int sizeOfTable = reg->sizeOfTable;
    for (int i = 0; i < sizeOfTable; i++) {
        createStampedValue(reg->a_table[i][i]->r_value, regSRSW->r_value->stamp,regSRSW->r_value->value);
    }
}