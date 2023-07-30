#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>
#include "../src/PSnapshot.h"
#include <assert.h>

#define NUM_THREADS 10
#define NUM_REGISTERS 3

void printSnap(snap pSnap, int numRegisters) {
    for (int i = 0; i < numRegisters; ++i) {
        printf("%d ", pSnap[i]);
    }
    printf("\n");
}

int main() {
{
    PSnapshot test4Snapshot;
    if(createPSnapshot(&test4Snapshot,1,2,-1) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    int snap1[1];
    int snap2[1];
    int registers[1];
    for(int j = 0;j<1;j++)registers[j] = j;

#pragma omp parallel default(none) shared(test4Snapshot,snap1,snap2,registers) num_threads(2)
{
    for(int i = 0;i<1000000;i++) {
    int me = omp_get_thread_num();
    if(me == 0){
        update(&test4Snapshot,0,10,me);
        // update(&test4Snapshot,1,10,me);
        p_snapshot(&test4Snapshot,snap1,registers,1);
    }
        if(me == 1){
        update(&test4Snapshot,0,11,me);
        // update(&test4Snapshot,1,11,me);
        p_snapshot(&test4Snapshot,snap2,registers,1);
        }
    #pragma omp barrier
    if(snap1[0] == 11 && snap2[0] == 10) assert(false);
    #pragma omp barrier
    }
}
    freePSnapshot(&test4Snapshot);
    printf("\nTest Linerizability single register passed.\n\n");

}

{
    PSnapshot test5Snapshot;
    if(createPSnapshot(&test5Snapshot,2,2,-1) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    int snap3[2];
    int snap4[2];
    int registers[2];
    for(int j = 0;j<2;j++)registers[j] = j;

#pragma omp parallel default(none) shared(test5Snapshot,snap3,snap4,registers) num_threads(2)
{
    for(int i = 0;i<1000000;i++) {
    int me = omp_get_thread_num();
    if(me == 0){
        update(&test5Snapshot,0,10,me);
        update(&test5Snapshot,1,10,me);
        p_snapshot(&test5Snapshot,snap3,registers,2);
    }
        if(me == 1){
        update(&test5Snapshot,0,11,me);
        update(&test5Snapshot,1,11,me);
        p_snapshot(&test5Snapshot,snap4,registers,2);
        }
    #pragma omp barrier
    if(snap3[0] == 10 && snap3[1] == 11 && (snap4[0]!=10 || snap4[1]!=11)) assert(false);
    if(snap3[0] == 11 && snap3[1] == 11 && (snap4[0]!=11 || snap4[1]!=11)) assert(false);
    if(snap4[0] == 10 && snap4[1] == 10 && (snap3[0]!=10 || snap3[1]!=10)) assert(false);
    if(snap3[0] == 10 && snap3[1] == 11 && snap4[0]==11 && snap4[1]==10) assert(false);
    if( (snap3[0] == snap4[0] && snap3[1] == snap4[1]) || 
        (snap3[0] == 10 && snap3[1] == 10 && snap4[0] == 10 && snap4[1] == 11) || 
        (snap3[0] == 11 && snap3[1] == 10 && snap4[0] == 11 && snap4[1] == 11) || 
        (snap3[0] == 11 && snap3[1] == 11 && snap4[0] == 11 && snap4[1] == 11) ||

        (snap3[0] == 11 && snap3[1] == 11 && snap4[0] == 10 && snap4[1] == 10) || 
        (snap3[0] == 11 && snap3[1] == 10 && snap4[0] == 10 && snap4[1] == 11) || 
        (snap3[0] == 10 && snap3[1] == 11 && snap4[0] == 10 && snap4[1] == 10) || 
        (snap3[0] == 10 && snap3[1] == 10 && snap4[0] == 10 && snap4[1] == 10) ||
        (snap3[0] == 10 && snap3[1] == 10 && snap4[0] == 11 && snap4[1] == 11)) {}
        else{printf("snap1,reg1:%d, snap1,reg2:%d, snap2,reg1:%d, snap2,reg2:%d\n",snap3[0],snap3[1],snap4[0],snap4[1]); assert(false);}
    #pragma omp barrier
    }
}

    printf("\nTest Linerizability double register passed.\n\n");
    freePSnapshot(&test5Snapshot);
}
}
