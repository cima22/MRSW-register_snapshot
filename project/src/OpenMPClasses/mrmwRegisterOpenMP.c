#include "mrmwRegisterOpenMP.h"


int createAtomicMRMWRegister(AtomicMRMWRegister* reg, int capacity, int init){
    reg= calloc(1, sizeof(AtomicMRMWRegister));
    if(reg==NULL) {
        fprintf(stderr,"Memory allocation failed for MRMW Register.\n");
        return EXIT_FAILURE;
    }
    for (int i = 0; i < capacity; ++i) {
        AtomicMRSWRegister mrsw = {NULL, 0};
        if(reg==NULL) {
            fprintf(stderr,"Memory allocation failed for MRMW Register.\n");
            return EXIT_FAILURE;
        }
        reg[i] = createAtomicMRSWRegister(&mrsw, init, capacity);
    }
    return EXIT_SUCCESS;
}

