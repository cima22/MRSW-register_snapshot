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

WFSnapshot* createWFSnapshot(int capacity, int init);

StampedSnap* collect(WFSnapshot* snapshot);
void update(WFSnapshot* snapshot, int value);
int* scan(WFSnapshot* snapshot);
void deleteWFSnapshot(WFSnapshot* snapshot);

#endif //AMP_TESTS_WFSNAPSHOT_H
