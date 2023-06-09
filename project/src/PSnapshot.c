//
// Created by Gabriele on 09/06/2023.
//

#include "PSnapshot.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "omp.h"

typedef struct{
    int threadID;
    int sn;
} regStamp;

int createPSnapshot(PSnapshot* snapshot, int capacity, int threadNum, int init) {
    if(snapshot == NULL) {
        fprintf(stderr, "Snapshot to create is null");
        return EXIT_FAILURE;
    }
    snapshot->capacity = capacity;
    snapshot->threadNum = threadNum;
    snapshot->reg = calloc(capacity, sizeof(Reg));
    if(snapshot->reg == NULL) {
        fprintf(stderr, "Memory allocation failed: register array");
        return EXIT_FAILURE;
    }
    snapshot->AS = calloc(capacity, sizeof(activeSet));
    if(snapshot->AS == NULL) {
        fprintf(stderr, "Memory allocation failed: active set array");
        return EXIT_FAILURE;
    }
    for (int i = 0; i < capacity; ++i) {
        snapshot->AS[i] = calloc(threadNum,sizeof(bool));
    }
    snapshot->ANNOUNCE = calloc(threadNum, sizeof(registerSet));
    if(snapshot->ANNOUNCE == NULL) {
        fprintf(stderr, "Memory allocation failed: ANNOUNCE array");
        return EXIT_FAILURE;
    }
    snapshot->HELPSNAP = calloc(threadNum,sizeof(snap*));
    if(snapshot->HELPSNAP == NULL) {
        fprintf(stderr, "Memory allocation failed: rows of HELPSNAP matrix");
        return EXIT_FAILURE;
    }
    for (int i = 0; i < threadNum; ++i) {
        snapshot->HELPSNAP[i] = calloc(threadNum, sizeof(snap));
        if(snapshot->HELPSNAP[i] == NULL) {
            fprintf(stderr, "Memory allocation failed: column of HELPSNAP matrix");
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

bool regsEqual(Reg r1, Reg r2){
    return r1.value == r2.value && r1.threadID == r2.threadID && r1.nbw == r2.nbw ? true : false;
}

int p_snapshot(PSnapshot* snapshot, snap pSnap, registerSet registers, int numRegisters){
    int me = omp_get_thread_num();
    snapshot->ANNOUNCE[me] = calloc(numRegisters, sizeof(Reg));
    if(snapshot->ANNOUNCE[me] == NULL){
        fprintf(stderr, "Memory allocation failed: ANNOUNCE[me] in p_snapshot()");
        return EXIT_FAILURE;
    }
    memcpy(snapshot->ANNOUNCE[me],registers,numRegisters * sizeof(Reg));
    regStamp can_help_me[snapshot->capacity];
    for (int i = 0; i < snapshot->capacity; ++i) {
        can_help_me[i].threadID = -1;
        can_help_me[i].sn = -1;
    }
    Reg aa[numRegisters];
    Reg bb[numRegisters];
    for (int i = 0; i < numRegisters; ++i) {
        (snapshot->AS[registers[i]])[me] = true;
    }
    for (int i = 0; i < numRegisters; ++i) {
        aa[i] = snapshot->reg[registers[i]];
    }
    while(true){
        for (int i = 0; i < numRegisters; ++i) {
            bb[i] = snapshot->reg[registers[i]];
        }
        bool areEqual = true;
        for (int i = 0; i < numRegisters && areEqual; ++i) {
            areEqual = regsEqual(aa[i],bb[i]);
        }
        if(areEqual){
            for (int i = 0; i < numRegisters; ++i) {
                (snapshot->AS[registers[i]])[me] = false;
                pSnap[i] = bb[i].value;
            }
            return EXIT_SUCCESS;
        }
        for (int i = 0; i < numRegisters; ++i) {
            if(!regsEqual(aa[i],bb[i])){
                can_help_me[registers[i]].threadID = bb[i].threadID;
                can_help_me[registers[i]].sn = bb[i].sn;
            }
        }

    }
}

void freePSnapshot(PSnapshot* snapshot){
    for (int i = 0; i < snapshot->threadNum; ++i) {
        for (int j = 0; j < snapshot->threadNum; ++j) {
            free(snapshot->HELPSNAP[i][j]);
        }
        free(snapshot->HELPSNAP[i])
    }
    free(snapshot->HELPSNAP);
    for (int i = 0; i < snapshot->threadNum; ++i) {
        free(snapshot->ANNOUNCE[i]);
    }
    free(snapshot->ANNOUNCE);
    for (int i = 0; i < snapshot->capacity; ++i) {
        free(snapshot->AS[i]);
    }
    free(snapshot->AS);
    free(snapshot->reg);
}
