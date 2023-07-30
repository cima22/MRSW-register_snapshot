//
// Created by Gabriele on 09/06/2023.
//

#ifndef MRSW_REGISTER_SNAPSHOT_PSNAPSHOT_H
#define MRSW_REGISTER_SNAPSHOT_PSNAPSHOT_H

#include "PSnapshot.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mrmwREG.h"

typedef bool* activeSet;
typedef atomic_int* snap;
typedef atomic_int* registerSet;

typedef struct {
    int capacity;
    int threadNum;
    mrmwREG* reg;
    activeSet* AS;
    registerSet* ANNOUNCE;
    int* ANNOUNCE_SIZES;
    snap** HELPSNAP;
} PSnapshot;

typedef struct{
    int pid;
    int sn;
} RegStamp;

typedef struct{
    RegStamp* regStamps;
    size_t size;
} RegStampCollection;

typedef struct {
    int* threads_to_help;
    int size;
}ToHelp;

typedef struct {
    int* registers_to_read;
    int size;
}ToRead;

int createPSnapshot(PSnapshot* snapshot, int capacity, int threadNum, int init);
int p_snapshot(PSnapshot* snapshot, int* pSnap, int* registers, int numRegisters);
int update(PSnapshot* snapshot, int r, int value, int threadID);
void freePSnapshot(PSnapshot* snapshot);
bool checkToHelpEmpty(bool* to_help, int threadNum);
bool isInAnnounce(int* announce,int rr, int capacity);
int get_sequence_number();

#endif //MRSW_REGISTER_SNAPSHOT_PSNAPSHOT_H
