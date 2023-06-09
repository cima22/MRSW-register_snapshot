//
// Created by Gabriele on 09/06/2023.
//

#include <stdbool.h>

#ifndef MRSW_REGISTER_SNAPSHOT_PSNAPSHOT_H
#define MRSW_REGISTER_SNAPSHOT_PSNAPSHOT_H

typedef struct {
    int value;
    int threadID;
    int sn;
} Reg;

typedef bool* activeSet;
typedef int* snap;
typedef int* registerSet;

typedef struct {
    int capacity;
    int threadNum;
    Reg* reg;
    activeSet* AS;
    registerSet* ANNOUNCE;
    snap** HELPSNAP;
} PSnapshot;

int createPSnapshot(PSnapshot* snapshot, int capacity, int threadNum, int init);
int p_snapshot(PSnapshot* snapshot, snap pSnap, registerSet registers, int numRegisters);
int update(PSnapshot* snapshot, int r, int value, int threadID);
void freePSnapshot(PSnapshot* snapshot);

#endif //MRSW_REGISTER_SNAPSHOT_PSNAPSHOT_H
