//
// Created by Gabriele on 01/06/2023.
//

#include "WFSnapshot.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "omp.h"

int createWFSnapshot(WFSnapshot* snapshot, int capacity, int init) {
    if(snapshot == NULL) {
        fprintf(stderr, "Snapshot to create is null");
        return EXIT_FAILURE;
    }
    snapshot->a_table = calloc(capacity, sizeof(StampedSnap));
    if(snapshot->a_table == NULL) {
        fprintf(stderr, "Memory allocation failed: stamped snap table");
        return EXIT_FAILURE;
    }
    snapshot->capacity = capacity;
    for (int i = 0; i < capacity; i++) {
        snapshot->a_table[i].stamp = 0;
        snapshot->a_table[i].value = init;
        int * snap = calloc(capacity,sizeof(int));
        if(snap == NULL){
            fprintf(stderr, "Memory allocation failed: snap in a_table");
            return EXIT_FAILURE;
        }
        snapshot->a_table[i].snap = snap;
    }
    return EXIT_SUCCESS;
}

int collect(WFSnapshot* snapshot, StampedSnap** copy){
    *copy = calloc(snapshot->capacity, sizeof(StampedSnap));
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
        (*copy)[j] = snapshot->a_table[j];
        (*copy)[j].snap = snap;
    }
    return EXIT_SUCCESS;
}

int update(WFSnapshot* snapshot, int value) {
    int me = omp_get_thread_num();
    if(scan(snapshot,snapshot->a_table[me].snap) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    snapshot->a_table[me].stamp++;
    snapshot->a_table[me].value = value;
    return EXIT_SUCCESS;
}

int scan(WFSnapshot* snapshot, int* snap) {
    StampedSnap* oldCopy;
    StampedSnap* newCopy;
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
            if (oldCopy[j].stamp != newCopy[j].stamp) {
                if (moved[j]) {
                    memcpy(snap,oldCopy[j].snap,snapshot->capacity * sizeof(int));
                    for (int i = 0; i < snapshot->capacity; ++i) {
                        free(oldCopy[i].snap);
                        free(newCopy[i].snap);
                    }
                    free(oldCopy);
                    free(newCopy);
                    return EXIT_SUCCESS;
                } else {
                    moved[j] = true;
                    for (int i = 0; i < snapshot->capacity; ++i) {
                        free(oldCopy[i].snap);
                    }
                    free(oldCopy);
                    oldCopy = newCopy;
                    continueWhile = true;
                }
            }
        }
        if(continueWhile)
            continue;
        for (int j = 0; j < snapshot->capacity; j++) {
            snap[j] = newCopy[j].value;
        }
        for (int i = 0; i < snapshot->capacity; ++i) {
            free(oldCopy[i].snap);
            free(newCopy[i].snap);
        }
        free(oldCopy);
        free(newCopy);
        return EXIT_SUCCESS;
    }
}

void freeWFSnapshot(WFSnapshot* snapshot) {
    for (int i = 0; i < snapshot->capacity; i++) {
       free(snapshot->a_table[i].snap);
    }
    free(snapshot->a_table);
}
