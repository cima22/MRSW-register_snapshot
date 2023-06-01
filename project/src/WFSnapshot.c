//
// Created by Gabriele on 01/06/2023.
//

#include "WFSnapshot.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "omp.h"

WFSnapshot* createWFSnapshot(int capacity, int init) {
    WFSnapshot* snapshot = (WFSnapshot*) malloc(sizeof(WFSnapshot));
    snapshot->a_table = (StampedSnap*)malloc(capacity * sizeof(StampedSnap));
    snapshot->capacity = capacity;

    for (int i = 0; i < capacity; i++) {
        snapshot->a_table[i].stamp = 0;
        snapshot->a_table[i].value = init;
        snapshot->a_table[i].snap = NULL;
    }

    return snapshot;
}

StampedSnap* collect(WFSnapshot* snapshot) {
    StampedSnap* copy = (StampedSnap*)malloc(snapshot->capacity * sizeof(StampedSnap));
    for (int j = 0; j < snapshot->capacity; j++) {
        copy[j] = snapshot->a_table[j];
    }
    return copy;
}

void update(WFSnapshot* snapshot, int value) {
    int me = omp_get_thread_num();
    int* snap = scan(snapshot);
    StampedSnap oldValue = snapshot->a_table[me];
    StampedSnap newValue;
    newValue.stamp = oldValue.stamp + 1;
    newValue.value = value;
    newValue.snap = snap;
    snapshot->a_table[me] = newValue;
}

int* scan(WFSnapshot* snapshot) {
    StampedSnap* oldCopy;
    StampedSnap* newCopy;
    bool moved[snapshot->capacity];
    oldCopy = collect(snapshot);
    while (true) {
        newCopy = collect(snapshot);
        for (int j = 0; j < snapshot->capacity; j++) {
            if (oldCopy[j].stamp != newCopy[j].stamp) {
                if (moved[j]) {
                    int* result = (int*) malloc(sizeof(int) * snapshot->capacity);
                    memcpy(result,oldCopy->snap,snapshot->capacity);
                    free(oldCopy);
                    free(newCopy);
                    return result;
                } else {
                    moved[j] = true;
                    free(oldCopy);
                    oldCopy = newCopy;
                    continue;
                }
            }
        }

        int* result = (int*)malloc(snapshot->capacity * sizeof(int));
        for (int j = 0; j < snapshot->capacity; j++) {
            result[j] = newCopy[j].value;
        }
        free(oldCopy);
        free(newCopy);
        return result;
    }
}

void deleteWFSnapshot(WFSnapshot* snapshot) {
    for (int i = 0; i < snapshot->capacity; i++) {
        free(snapshot->a_table[i].snap);
    }
   // free(snapshot->a_table);
    free(snapshot);
}