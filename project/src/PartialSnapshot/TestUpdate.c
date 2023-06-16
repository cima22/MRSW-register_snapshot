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
    int init = 0;
    PSnapshot snapshot;
    if(createPSnapshot(&snapshot, NUM_REGISTERS, NUM_THREADS, init) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    omp_set_num_threads(NUM_THREADS);
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();

        //  unsigned int rand_seed = thread_id + 1;

        // int num_registers = rand_r(&rand_seed) % (NUM_REGISTERS + 1); // random number from 0 to NUM_REGISTERS
        // if(num_registers == 0) num_registers++;

        // int* registerSet = calloc(num_registers, sizeof(int));
        // if(registerSet == NULL){
        //     fprintf(stderr, "Memory allocation failed: registerSet in main()");
        //     exit(EXIT_FAILURE);
        // }

        // for(int i = 0; i < num_registers; i++) {
        //     while(true) {
        //     int aa = rand_r(&rand_seed) % (NUM_REGISTERS); // Random register index
        //     bool wasInVect = false;
        //     for(int j = 0;j<i;j++){
        //         if(aa == registerSet[j]){wasInVect = true; break;}
        //     }
        //     if(wasInVect == false) {
        //     registerSet[i] = aa;
        //     break;
        //     }
        //     }
        // }
        int registerSet[] = {0, 1, 2}; // array containing 0-9

        int num_registers = sizeof(registerSet) / sizeof(registerSet[0]); 
        int* pSnap = calloc(num_registers, sizeof(snap));
        if(pSnap == NULL){
            fprintf(stderr, "Memory allocation failed: pSnap in main()");
            exit(EXIT_FAILURE);
        }


        // Write to multiple registers
        // for(int j = 0;j<5;j++) {
        for(int i = 0; i < NUM_REGISTERS; i++){
            if(update(&snapshot, i, (thread_id+1)*10 + 1, thread_id) == EXIT_FAILURE)
                exit(EXIT_FAILURE);
        }

        if(p_snapshot(&snapshot, pSnap, registerSet, num_registers) == EXIT_FAILURE)
            exit(EXIT_FAILURE);
        #pragma omp critical
        {
        printf("Snapshot from thread %d:\n", thread_id);
        for (int i = 0; i < num_registers; ++i) {
            printf("register %d, having value %d \n",registerSet[i], pSnap[i]);
        }
        printf("\n");
        }
        
        free(pSnap);
    }
    freePSnapshot(&snapshot);
    return EXIT_SUCCESS;
}