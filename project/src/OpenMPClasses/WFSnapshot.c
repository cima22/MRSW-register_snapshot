//
// Created by Gabriele on 01/06/2023.
//

#include "WFSnapshot.h"

int createWFSnapshot(WFSnapshot* snapshot, int capacity, int init) {

    if(snapshot == NULL) {
        fprintf(stderr, "Snapshot to create is null");
        return EXIT_FAILURE;
    }

    snapshot->a_table = (ModifiedMRSW*)calloc(capacity, sizeof(ModifiedMRSW));
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
        createAtomicMRSWRegister(&(snapshot->a_table[i].mrswReg),init, capacity);
    }
    snapshot->capacity = capacity;
    return EXIT_SUCCESS;
}

int collect(WFSnapshot* snapshot, ModifiedMRSW** copy){
    *copy = calloc(snapshot->capacity, sizeof(ModifiedMRSW));
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

        (*copy)[j].snap = snap;
        CopyContentMRSWRegister(&((*copy)[j].mrswReg), &(snapshot->a_table[j].mrswReg));
    }
    return EXIT_SUCCESS;
}

int update(WFSnapshot* snapshot, int value, long*ThreadLastStamp) {
    int me = omp_get_thread_num();
    if(scan(snapshot,snapshot->a_table[me].snap) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }

    writeMRSW(&(snapshot->a_table[me].mrswReg), ThreadLastStamp,value);

    return EXIT_SUCCESS;
}

int scan(WFSnapshot* snapshot, int* snap) {
    ModifiedMRSW* oldCopy;
    ModifiedMRSW* newCopy;
    bool moved[snapshot->capacity];
    memset(moved,false,snapshot->capacity*sizeof(bool));

    if(collect(snapshot,&oldCopy) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    // Uncomment for further testing 
    // if(omp_get_thread_num() == 0)
    //     sleep(3);
    while (true) {
        bool continueWhile = false;
        if(collect(snapshot,&newCopy) == EXIT_FAILURE){
            return EXIT_FAILURE;
        }
        for (int j = 0; j < snapshot->capacity; j++) {
            
            AtomicSRSWRegister old_cpySRSW;
            MaxMRSW(&(oldCopy[j].mrswReg), &old_cpySRSW);
            AtomicSRSWRegister new_cpySRSW;
            MaxMRSW(&(newCopy[j].mrswReg), &new_cpySRSW);
            if ((old_cpySRSW.r_value->stamp) != (new_cpySRSW.r_value->stamp)) {
                if (moved[j]) {
                    memcpy(snap,oldCopy[j].snap,snapshot->capacity * sizeof(int));
                    // for (int i = 0; i < snapshot->capacity; ++i) {
                    //     free(oldCopy[i].snap);
                    //     free(newCopy[i].snap);
                    // }
                    // free(oldCopy);
                    // free(newCopy);
                    printf("Went in the moved\n");
                    return EXIT_SUCCESS;
                } else {
                    printf("Went into the else\n");
                    moved[j] = true;
                    // for (int i = 0; i < snapshot->capacity; ++i) {
                    //     free(oldCopy[i].snap);
                    // }
                    // free(oldCopy);
                    // Uncomment for further testing
                    // sleep(3);
                    oldCopy = newCopy;
                    continueWhile = true;
                }
            }
        }
        if(continueWhile)
            continue;
        for (int j = 0; j < snapshot->capacity; j++) {

            AtomicSRSWRegister new_cpySRSW;
            MaxMRSW(&(newCopy[j].mrswReg), &new_cpySRSW);
            snap[j] = new_cpySRSW.r_value->value;
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
