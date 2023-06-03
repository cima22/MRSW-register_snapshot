//
// Created by Gabriele on 01/06/2023.
//

#ifndef AMP_TESTS_WFSNAPSHOT_H
#define AMP_TESTS_WFSNAPSHOT_H

typedef struct {
    long stamp;
    int value;
    int* snap;
} StampedSnap;

typedef struct {
    StampedSnap* a_table;
    int capacity;
} WFSnapshot;

int createWFSnapshot(WFSnapshot* snapshot, int capacity, int init);
int collect(WFSnapshot* snapshot, StampedSnap** copy);
int update(WFSnapshot* snapshot, int value);
int scan(WFSnapshot* snapshot,int* snap);
void freeWFSnapshot(WFSnapshot* snapshot);

#endif //AMP_TESTS_WFSNAPSHOT_H
