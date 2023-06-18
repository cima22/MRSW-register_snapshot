#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>
#include "PSnapshot.h"

#define NUM_THREADS 10
#define NUM_REGISTERS 3

void printSnap(snap pSnap, int numRegisters) {
    for (int i = 0; i < numRegisters; ++i) {
        printf("%d ", pSnap[i]);
    }
    printf("\n");
}

int main() {
//         // Test linear


//         printf("\nTest linear with 2 threads.\n");
//         PSnapshot test2Snapshot;
//         if(createPSnapshot(&test2Snapshot, 2, 2, 1) == EXIT_FAILURE){
//             return EXIT_FAILURE;
//         }
//         int snap1[1];
//         int snap2[1];

//         int registers1[1];
//         int registers2[1];
//         registers1[0] = 0;
//         registers2[1] = 0;
//         int registers[2];
//         for(int j = 0;j<2;j++) registers[j] = j;

//         // int maximum_threadNR = 0;
// #pragma omp parallel default(none) shared(test2Snapshot) num_threads(i)
//         {
//             for(int k = 0;k<10000;k++) {
            
//             if(omp_get_thread_num() == 1) {
//             update(&test2Snapshot,0,omp_get_thread_num(), omp_get_thread_num());
//             p_snapshot(&test2Snapshot,snap2, registers2, 1);
//             }

//             if(omp_get_thread_num() == 0) {
//             update(&test2Snapshot,1,omp_get_thread_num(), omp_get_thread_num());
//             p_snapshot(&test2Snapshot,snap1, registers1, 1);
//             }
//             #pragma omp barrier
//             if()

//             }
//         }
// #pragma omp barrier
//         p_snapshot(&test2Snapshot,snap2, registers, i);
//         printf("\nSecond scan returns: ");
//         for (int j = 0; j < i; ++j) {
//             printf("[%d] ",snap2[j]);
//         }

//         freePSnapshot(&test2Snapshot);

//         assert(memcmp(snap1,expected1,sizeof(expected1)) == 0 && memcmp(snap2,expected2,sizeof(expected2)) == 0);
//         printf("\nTest 2 with %d threads has passed.\n",i);
//     }
//     printf("\nTest 2 passed.\n\n");
//     return EXIT_SUCCESS;
}