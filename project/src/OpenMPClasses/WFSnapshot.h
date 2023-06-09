//
// Created by Gabriele on 01/06/2023.
//

#ifndef AMP_TESTS_WFSNAPSHOT_H
#define AMP_TESTS_WFSNAPSHOT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "omp.h"
#include "mrswRegisterOpenMP.h"

typedef struct {
    AtomicMRSWRegister mrswReg;
    int* snap;
} ModifiedMRSW;

typedef struct {
    ModifiedMRSW* a_table;
    int capacity;
} WFSnapshot;

int createWFSnapshot(WFSnapshot* snapshot, int capacity, int init);
int collect(WFSnapshot* snapshot, ModifiedMRSW** copy);
int update(WFSnapshot* snapshot, int value, long*ThreadLastStamp);
int scan(WFSnapshot* snapshot,int* snap);
void freeWFSnapshot(WFSnapshot* snapshot);

#endif //AMP_TESTS_WFSNAPSHOT_H
