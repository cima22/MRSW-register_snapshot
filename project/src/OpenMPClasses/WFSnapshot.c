//
// Created by Gabriele on 01/06/2023.
//

#include "WFSnapshot.h"

int createWFSnapshot(WFSnapshot* snapshot, int capacity, int init) {

    if(snapshot == NULL) {
        fprintf(stderr, "Snapshot to create is null");
        return EXIT_FAILURE;
    }

    snapshot->a_table = (ModifiedMRMW*)calloc(capacity, sizeof(ModifiedMRMW));
    if(snapshot->a_table == NULL) {
        fprintf(stderr, "Memory allocation failed: stamped snap table");
        return EXIT_FAILURE;
    }

    for(int i = 0;i<capacity;i++) {
        snapshot->a_table[i].snap = (int*)calloc(capacity,sizeof(int));
        if(snapshot->a_table->snap == NULL) {
            fprintf(stderr, "Memory allocation failed: stamped snap");
            return EXIT_FAILURE;
        }
        // int numberReaders = omp_get_num_threads();
        createAtomicMRSWRegister(&(snapshot->a_table[i].mrswReg),init, capacity);
    }
    snapshot->capacity = capacity;
    return EXIT_SUCCESS;
}

int collect(WFSnapshot* snapshot, ModifiedMRMW** copy){
    *copy = calloc(snapshot->capacity, sizeof(ModifiedMRMW));
    if(*copy == NULL){
        fprintf(stderr, "Memory allocation failed: copy array in collect()");
        return EXIT_FAILURE;
    }
    for (int j = 0; j < snapshot->capacity; j++) {
        int* snap = calloc(snapshot->capacity,sizeof(int));
        if(snap == NULL){
            fprintf(stderr, "Memory allocation failed: snap of copy in collect()");
            return EXIT_FAILURE;
        }
        memcpy(snap,snapshot->a_table[j].snap,snapshot->capacity*sizeof(int));
        // (*copy)[j] = snapshot->a_table[j];
        // (*copy)[j].snap = snap;
        CopyContentMRSWRegister(&((*copy)[j].mrswReg), &(snapshot->a_table[j].mrswReg));
    }
    return EXIT_SUCCESS;
}

int update(WFSnapshot* snapshot, int value, long*ThreadLastStamp) {
    int me = omp_get_thread_num();
    // long ThreadLastStamp = (long)me;
    if(scan(snapshot,snapshot->a_table[me].snap) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    // printf("\nthe value to write is %d:",value);
    writeMRSW(&(snapshot->a_table[me].mrswReg), &(ThreadLastStamp[me]),value);
    AtomicSRSWRegister* old_cpySRSW = (AtomicSRSWRegister*)calloc(1, sizeof(AtomicSRSWRegister));
    #pragma omp barrier
    MaxMRSW(&(snapshot->a_table[me].mrswReg), old_cpySRSW);
    // printf("\n In update : old stamp %ld old value %d", old_cpySRSW->r_value->stamp, old_cpySRSW->r_value->value);
    // snapshot->a_table[me].stamp++;
    // snapshot->a_table[me].value = value;
    return EXIT_SUCCESS;
}

int scan(WFSnapshot* snapshot, int* snap) {
    ModifiedMRMW* oldCopy;
    ModifiedMRMW* newCopy;
    bool moved[snapshot->capacity];
    memset(moved,false,snapshot->capacity*sizeof(bool));

    if(collect(snapshot,&oldCopy) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    while (true) {
        bool continueWhile = false;
        if(collect(snapshot,&newCopy) == EXIT_FAILURE){
            return EXIT_FAILURE;
        }
        for (int j = 0; j < snapshot->capacity; j++) {
            AtomicSRSWRegister* old_cpySRSW = (AtomicSRSWRegister*)calloc(1, sizeof(AtomicSRSWRegister));
            MaxMRSW(&(oldCopy[j].mrswReg), old_cpySRSW);
            AtomicSRSWRegister* new_cpySRSW = (AtomicSRSWRegister*)calloc(1, sizeof(AtomicSRSWRegister));
            MaxMRSW(&(newCopy[j].mrswReg), new_cpySRSW);
            // printf("\n In Scan of j:%d : old stamp %ld old value %d; new stamp %ld new value %d",j, old_cpySRSW->r_value->stamp, old_cpySRSW->r_value->value,
                                                                                // new_cpySRSW->r_value->stamp, new_cpySRSW->r_value->value);
            if ((old_cpySRSW->r_value->stamp) != (new_cpySRSW->r_value->stamp)) {
                if (moved[j]) {
                    memcpy(snap,oldCopy[j].snap,snapshot->capacity * sizeof(int));
                    // for (int i = 0; i < snapshot->capacity; ++i) {
                    //     free(oldCopy[i].snap);
                    //     free(newCopy[i].snap);
                    // }
                    // free(oldCopy);
                    // free(newCopy);
                    return EXIT_SUCCESS;
                } else {
                    moved[j] = true;
                    // for (int i = 0; i < snapshot->capacity; ++i) {
                    //     free(oldCopy[i].snap);
                    // }
                    // free(oldCopy);
                    oldCopy = newCopy;
                    continueWhile = true;
                }
            }
        }
        if(continueWhile)
            continue;
        for (int j = 0; j < snapshot->capacity; j++) {
            AtomicSRSWRegister* new_cpySRSW = (AtomicSRSWRegister*)calloc(1, sizeof(AtomicSRSWRegister));
            MaxMRSW(&(newCopy[j].mrswReg), new_cpySRSW);
            snap[j] = new_cpySRSW->r_value->value;
        }
        // for (int i = 0; i < snapshot->capacity; ++i) {
        //     free(oldCopy[i].snap);
        //     free(newCopy[i].snap);
        // }
        // free(oldCopy);
        // free(newCopy);
        return EXIT_SUCCESS;
    }
}

void freeWFSnapshot(WFSnapshot* snapshot) {
    for (int i = 0; i < snapshot->capacity; i++) {
       free(snapshot->a_table[i].snap);
    }
    free(snapshot->a_table);
}
