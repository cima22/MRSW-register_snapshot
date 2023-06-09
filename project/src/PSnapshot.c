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
} RegStamp;

typedef struct{
    RegStamp* regStamps;
    size_t size;
} RegStampCollection;

int createRegStampCollection(RegStampCollection* collection){
    if(collection == NULL) {
        fprintf(stderr, "Register Stamp collection to create is null");
        return EXIT_FAILURE;
    }
    collection->regStamps = NULL;
    collection->size = 0;
    return EXIT_SUCCESS;
}

void freeRegStampCollection(RegStampCollection* collection){
    free(collection->regStamps);
}

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
    for (int i = 0; i < capacity; ++i) {
        snapshot->reg[i].value = init;
        snapshot->reg[i].threadID = -1;
        snapshot->reg[i].sn = -1;
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
    return r1.value == r2.value && r1.threadID == r2.threadID && r1.sn == r2.sn;
}

int threadMoved(RegStampCollection* can_help_me){
    RegStamp* regStamps = can_help_me->regStamps;
    for (int i = 0; i < can_help_me->size; ++i) {
        for (int j = i + 1; j < can_help_me->size; ++j) {
            if(regStamps[i].threadID == regStamps[j].threadID &&
                regStamps[i].sn != regStamps[j].sn){
                return regStamps[i].threadID;
            }
        }
    }
    return -1;
}

int p_snapshot(PSnapshot* snapshot, snap pSnap, registerSet registers, int numRegisters){
    int me = omp_get_thread_num();
    snapshot->ANNOUNCE[me] = calloc(numRegisters, sizeof(Reg));
    if(snapshot->ANNOUNCE[me] == NULL){
        fprintf(stderr, "Memory allocation failed: ANNOUNCE[me] in p_snapshot()");
        return EXIT_FAILURE;
    }
    memcpy(snapshot->ANNOUNCE[me],registers,numRegisters * sizeof(Reg));
    RegStampCollection can_help_me;
    if(createRegStampCollection(&can_help_me) == EXIT_FAILURE){
        return EXIT_FAILURE;
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
        for (int i = 0; (i < numRegisters) && areEqual; ++i) {
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
                int idx = can_help_me.size;
                can_help_me.size = idx + 1;
                can_help_me.regStamps = realloc(can_help_me.regStamps,can_help_me.size * sizeof(RegStamp));
                if(can_help_me.regStamps == NULL){
                    fprintf(stderr, "Memory allocation failed: can_help_me");
                    return EXIT_FAILURE;
                }
                can_help_me.regStamps[idx].threadID = bb[i].threadID;
                can_help_me.regStamps[idx].sn = bb[i].sn;
            }
        }
        int threadMovedId = threadMoved(&can_help_me);
        if(threadMovedId != -1){
            for (int j = 0; j < numRegisters; ++j) {
                (snapshot->AS[registers[j]])[me] = false;
            }
            memcpy(pSnap,snapshot->HELPSNAP[threadMovedId][me],numRegisters * sizeof(int));
            freeRegStampCollection(&can_help_me);
            return EXIT_SUCCESS;
        }
        memcpy(aa,bb,sizeof(bb));
    }
}

void freePSnapshot(PSnapshot* snapshot){
    for (int i = 0; i < snapshot->threadNum; ++i) {
        for (int j = 0; j < snapshot->threadNum; ++j) {
            free(snapshot->HELPSNAP[i][j]);
        }
        free(snapshot->HELPSNAP[i]);
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
