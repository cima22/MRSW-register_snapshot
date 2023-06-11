//
// Created by Gabriele on 09/06/2023.
//

#include "PSnapshot.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mrmwREG.h"

typedef struct{
    int pid;
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
    snapshot->reg = calloc(capacity, sizeof(mrmwREG));
    if(snapshot->reg == NULL) {
        fprintf(stderr, "Memory allocation failed: register array");
        return EXIT_FAILURE;
    }
    for (int i = 0; i < capacity; ++i) {
        initREG(&snapshot->reg[i], init, -1, -1);
        // this usage of the tmp: the getValue
        // function was giving a strange error of type mrmwREG incompatible with type mrmwREG
        mrmwREG tmp;
        copyREG(&tmp, &(snapshot->reg[i]));
        fprintf(stdout, "%d\n", getValue(tmp));
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

int threadMoved(RegStampCollection* can_help_me){
    RegStamp* regStamps = can_help_me->regStamps;
    for (int i = 0; i < can_help_me->size; ++i) {
        for (int j = i + 1; j < can_help_me->size; ++j) {
            if(regStamps[i].pid == regStamps[j].pid &&
               regStamps[i].sn != regStamps[j].sn){
                return regStamps[i].pid;
            }
        }
    }
    return -1;
}

int p_snapshot(PSnapshot* snapshot, snap pSnap, registerSet registers, int numRegisters){
    int me = omp_get_thread_num();
    snapshot->ANNOUNCE[me] = calloc(numRegisters, sizeof(mrmwREG));
    if(snapshot->ANNOUNCE[me] == NULL){
        fprintf(stderr, "Memory allocation failed: ANNOUNCE[me] in p_snapshot()");
        return EXIT_FAILURE;
    }
    memcpy(snapshot->ANNOUNCE[me],registers,numRegisters * sizeof(mrmwREG));
    RegStampCollection can_help_me;
    if(createRegStampCollection(&can_help_me) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    mrmwREG aa[numRegisters];
    mrmwREG bb[numRegisters];
    for (int i = 0; i < numRegisters; ++i) {
        (snapshot->AS[registers[i]])[me] = true;
    }
    for (int i = 0; i < numRegisters; ++i) {
        copyREG(&(aa[i]),&(snapshot->reg[registers[i]]));
    }
    while(true){
        for (int i = 0; i < numRegisters; ++i) {
            copyREG(&(bb[i]), &(snapshot->reg[registers[i]]));
        }
        bool areEqual = true;
        for (int i = 0; (i < numRegisters) && areEqual; ++i) {
            areEqual = compareREG(aa[i],bb[i]);
        }
        if(areEqual){
            for (int i = 0; i < numRegisters; ++i) {
                (snapshot->AS[registers[i]])[me] = false;
                pSnap[i] = bb[i].value;
            }
            return EXIT_SUCCESS;
        }
        for (int i = 0; i < numRegisters; ++i) {
            if(!compareREG(aa[i],bb[i])){
                int idx = can_help_me.size;
                can_help_me.size = idx + 1;
                can_help_me.regStamps = realloc(can_help_me.regStamps,can_help_me.size * sizeof(RegStamp));
                if(can_help_me.regStamps == NULL){
                    fprintf(stderr, "Memory allocation failed: can_help_me");
                    return EXIT_FAILURE;
                }
                can_help_me.regStamps[idx].pid = bb[i].pid;
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

int get_sequence_number() {
    static int seq = 0;
    return ++seq;
}

// if to_help[i] == true, means that there are still threads that need
// to be helped
bool checkToHelpEmpty(bool* to_help, int threadNum) {
    for(int i = 0;i<threadNum;i++) {
        if(to_help[i] == true)
        return false;
    }
    return true;
}
bool isInAnnounce(registerSet announce,int rr, int capacity) {
    for(int i = 0;i<capacity;i++)
        if(announce[i] == rr)
            return true;
    return false;
}

int update(PSnapshot* snapshot,int r,int value,int ThreadID) {
    // line 1
    int threadNum = snapshot->threadNum;
    int capacity = snapshot->capacity;
    int nbw = get_sequence_number();
    updateREG(&(snapshot->reg[r]),value,nbw);
    
    // line 2
    activeSet readers[threadNum];
    memcpy(readers, snapshot->AS[r], sizeof(bool) * threadNum);
    registerSet* announce = calloc(threadNum, sizeof(registerSet));
    if (announce == NULL) {
        fprintf(stderr, "Memory allocation failed: announce in update()");
        return;
    }
    for (int j = 0;j<threadNum;j++) {
        announce[j] = calloc(capacity,sizeof(int));
        if(announce[j] == NULL) {
            fprintf(stderr, "Memory allocation failed: announce[%d] in update()", j);
            return;
        }
    }
    // line 05 begin
    bool to_help[threadNum];
    for(int i = 0;i<threadNum;i++) to_help[i] = false;

    for(int j = 0;j<threadNum;j++) {
        if (readers[j] == true) {
            memcpy(announce[j], snapshot->ANNOUNCE[j], capacity * sizeof(int));
            if(announce[j][r] == true) 
                to_help[j] = true;
        }
    }
    if (checkToHelpEmpty(to_help, threadNum))
        return EXIT_SUCCESS;
    // line 07 end

    // line 08
    // is register rr needed?
    bool to_read[capacity];
    for (int i = 0;i<capacity;i++) to_read[i] = false;
    for(int i = 0;i<threadNum;i++) {
        for (int j = 0;j<capacity; j++) {
            if(announce[i][j] == true)
            to_read[j] = true;
        }
    }

    // line 09
    RegStampCollection can_help[threadNum];
    for (int i = 0;i<threadNum;i++)
        if(createRegStampCollection(&can_help[i]) == EXIT_FAILURE){
        return EXIT_FAILURE;
        }
    // line 10
    mrmwREG aa[capacity];
    mrmwREG bb[capacity];
    for (int rr = 0;rr<capacity;rr++){
        if(to_read[rr] == true)
        // here a function for copying structs is needed
        copyREG(&(aa[rr]), &(snapshot->reg[rr]));
    }
    // begin line 11
    while (!checkToHelpEmpty(to_help,threadNum)) {
        for(int i = 0;i<capacity;i++)
            if(to_read[i] == true) 
                copyREG(&(bb[i]), &(snapshot->reg[i]));

        bool still_to_help[threadNum];
        for(int i = 0;i<threadNum;i++) still_to_help[i] = false;

        for(int rr = 0;rr<capacity;rr++)
            if(to_read[rr] == true)
                if(!compareREG(aa[rr],bb[rr])) {

                for(int j = 0;j<threadNum;j++)
                    if(to_help[j] == true)
                        if(isInAnnounce(announce[j], rr, capacity)) {
                            still_to_help[j] = true;
                            int idx = can_help[j].size;
                            can_help[j].size = idx + 1;
                            can_help[j].regStamps = realloc(can_help[j].regStamps,can_help[j].size * sizeof(RegStamp));
                            if(can_help[j].regStamps == NULL){
                                fprintf(stderr, "Memory allocation failed: can_help_me");
                                return EXIT_FAILURE;
                            }
                            can_help[j].regStamps[idx].pid = bb[r].pid;
                            can_help[j].regStamps[idx].sn = bb[r].sn;
                }
            }
        
        for(int j = 0;j<threadNum; j++)
            if(to_help[j] == true && still_to_help[j] == false) {
                snapshot->HELPSNAP[ThreadID][j] = calloc(capacity,sizeof(int));
                for(int r = 0;r<capacity;r++) {
                    snapshot->HELPSNAP[ThreadID][j][r] = bb[r].value;
                }
            }
        for(int j = 0;j<threadNum;j++) {
            int threadMovedId = threadMoved(&can_help[j]);
            if(threadMovedId!=-1) {
                memcpy(snapshot->HELPSNAP[ThreadID][j], snapshot->HELPSNAP[threadMovedId][j], capacity*sizeof(int));
                still_to_help[j] = false;
            }
        }
        memcpy(to_help,still_to_help,threadNum*sizeof(bool));
        memcpy(aa,bb,capacity * sizeof(mrmwREG));

        for(int j = 0;j<capacity;j++)to_read[j] = false;
        for(int i = 0;i<threadNum;i++) {
            for (int j = 0;j<capacity; j++) {
                if(announce[i][j] == true)
                to_read[j] = true;
            }
        }
    }
    return EXIT_SUCCESS;

    
}