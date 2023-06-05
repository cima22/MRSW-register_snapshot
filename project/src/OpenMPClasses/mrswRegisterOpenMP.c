#include "mrswRegisterOpenMP.h"

void createAtomicMRSWRegister(AtomicMRSWRegister* reg, int init,  int readers) {
    reg->a_table = (AtomicSRSWRegister**) calloc(readers, sizeof(AtomicSRSWRegister*));
    for(int i = 0;i<readers;i++){
        reg->a_table[i] = (AtomicSRSWRegister*) calloc(readers, sizeof(AtomicSRSWRegister));
        for(int j = 0;j<readers;j++) {
            // reg->a_table[i][j] = (AtomicSRSWRegister*)calloc(1, sizeof(AtomicSRSWRegister));
            createAtomicSRSWRegister(&(reg->a_table[i][j]),init);
        }
    }
    reg->sizeOfTable = readers;
}


int readMRSW(AtomicMRSWRegister* reg) {
    int me = omp_get_thread_num();

    // Here I just copied the content of the SRSW register, did not create a pointer to the exact same location
    AtomicSRSWRegister* value = (AtomicSRSWRegister*)calloc(1, sizeof(AtomicSRSWRegister));
    value->r_value = (StampedValue*)calloc(1, sizeof(StampedValue));
    createStampedValue(value->r_value,reg->a_table[me][me].r_value->stamp, reg->a_table[me][me].r_value->value);

    int sizeOfTable = reg->sizeOfTable;
    for (int i = 0; i < (int) sizeOfTable; i++) {
        if(isFirstBigger(value->r_value, reg->a_table[i][me].r_value)){

            createStampedValue(value->r_value, reg->a_table[i][me].r_value->stamp, reg->a_table[i][me].r_value->value);
        }
    }
    for (int i = 0; i < sizeOfTable; i++) {
        createStampedValue(reg->a_table[me][i].r_value,value->r_value->stamp, value->r_value->value);
    }
    // The book is strange, I don't understand why it says to return an object of type StampedValue,
    // but the function is of type int ???
    return value->r_value->value;
}

void writeMRSW(AtomicMRSWRegister* reg, long* ThreadLastStamp,int v) {
    long stamp = *ThreadLastStamp + 1;
    *ThreadLastStamp = stamp;
    AtomicSRSWRegister* regSRSW = (AtomicSRSWRegister*)calloc(1, sizeof(AtomicSRSWRegister));
    createAtomicSRSWRegister(regSRSW,v);

    int sizeOfTable = reg->sizeOfTable;
    for (int i = 0; i < sizeOfTable; i++) {
        createStampedValue(reg->a_table[i][i].r_value, stamp,regSRSW->r_value->value);
    }
}

void CopyContentMRSWRegister(AtomicMRSWRegister* x, AtomicMRSWRegister* y) {
    if (x == NULL) {
        fprintf(stderr, "NULL pointer received in CopyContentMRSWRegister: first element (X)\n");
        return;
    }
    if (y == NULL) {
        fprintf(stderr, "NULL pointer received in CopyContentMRSWRegister: first element (Y)\n");
        return;
    }

    // Check if a_table is properly initialized in y
    if (y->a_table == NULL) {
        fprintf(stderr, "a_table not initialized in y\n");
        return;
    }

    // Initialize x's sizeOfTable and a_table if necessary
    if (x->a_table == NULL) {
        x->sizeOfTable = y->sizeOfTable;
        x->a_table = (AtomicSRSWRegister**) calloc(x->sizeOfTable, sizeof(AtomicSRSWRegister*));
        if (x->a_table == NULL) {
            fprintf(stderr, "Memory allocation for a_table in x failed\n");
            return;
        }
        for (int i = 0; i < x->sizeOfTable; i++) {
            x->a_table[i] = (AtomicSRSWRegister*) calloc(x->sizeOfTable, sizeof(AtomicSRSWRegister));
            if (x->a_table[i] == NULL) {
                fprintf(stderr, "Memory allocation for a_table[%d] in x failed\n", i);
                return;
            }
        }
    }
    x->sizeOfTable = y->sizeOfTable;
    // Copy y's a_table to x's a_table
    for (int i = 0; i < y->sizeOfTable; i++) {
        for (int j = 0; j < y->sizeOfTable; j++) {
            CopyContentSRSWRegister(&(x->a_table[i][j]), &(y->a_table[i][j]));
        }
    }
}

int MaxMRSW(AtomicMRSWRegister* reg, AtomicSRSWRegister* returnedReg) {

    if(reg == NULL){
        fprintf(stderr, "MaxMRSW: MRSW register was NULL!!\n");
        return EXIT_FAILURE;
    }
    
    if(returnedReg == NULL) {
        fprintf(stderr, "MaxMRSW: Returned register was NULL!!\n");
        return EXIT_FAILURE;
    }

    returnedReg->r_value = (StampedValue*)calloc(1,sizeof(StampedValue));
    if(returnedReg->r_value == NULL) {
        fprintf(stderr, "MaxMRSW: Memory allocation for stamped value failed!!\n");
        return EXIT_FAILURE;
    }
    initStampedValue(returnedReg->r_value,0);
    // printf("\n size of table %d \n",reg->sizeOfTable);
    for(int i = 0; i < reg->sizeOfTable; i++) {
        
        if(reg->a_table[i] == NULL) {
            fprintf(stderr, "MaxMRSW: Row %d of MRSW table was NULL!!\n", i);
            return EXIT_FAILURE;
        }
        for(int j = 0; j < reg->sizeOfTable; j++) {
            // printf("\nMaxMRSW: In the Max function, position:%d %d ; stamp, value are: %ld %d",i,j,reg->a_table[i][j].r_value->stamp,reg->a_table[i][j].r_value->value);
            if(reg->a_table[i][j].r_value->stamp > returnedReg->r_value->stamp){
                returnedReg->r_value->stamp = reg->a_table[i][j].r_value->stamp;
                returnedReg->r_value->value = reg->a_table[i][j].r_value->value;
            }
        }
    }
    return EXIT_SUCCESS;
}
