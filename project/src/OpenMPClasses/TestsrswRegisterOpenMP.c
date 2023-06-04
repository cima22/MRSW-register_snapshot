#include "srswRegisterOpenMP.h"
#include <stdio.h>
#include<omp.h>
#include <time.h>
#include <stdlib.h>
void reader(AtomicSRSWRegister* reg,ThreadSpecificSRSW* ourThread) {
    for (int i = 0; i < 20; i++) {
        int value = (int)readSRSW(reg,ourThread);
        printf("Reader thread: Read value %d\n", value);
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 10 * 1000 * 1000;  // 100,00 microseconds = 10 millisecond
        nanosleep(&ts, NULL);
    }
}

void writer(AtomicSRSWRegister* reg,ThreadSpecificSRSW* ourThread) {
    for (int i = 0; i < 20; i++) {
        int new_value = i;
        writeSRSW(reg, ourThread, new_value);
        printf("Writer thread: Wrote value %d\n", new_value);
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 10 * 1000 * 1000;  // 100,00 microseconds = 10 millisecond
        nanosleep(&ts, NULL);
    }
}

int main() {
    int initial_value = 0;
    AtomicSRSWRegister* reg = (AtomicSRSWRegister*)malloc(sizeof(AtomicSRSWRegister));
    createAtomicSRSWRegister(reg,initial_value);
    
    #pragma omp parallel num_threads(2)
    {
        ThreadSpecificSRSW* ourThread = (ThreadSpecificSRSW*)calloc(1, sizeof(ThreadSpecificSRSW*));
        createAtomicSRSWThreadSpecific(ourThread,reg->r_value);
        if (omp_get_thread_num() == 0) {
            reader(reg, ourThread);
        } else {
            writer(reg, ourThread);
        }
    }

    // int* last_value = (int*)readSRSW(reg,);
    // printf("Last read value: %d\n", *last_value);
    // not yet destroying, no freeing memory baby
    //destroyAtomicSRSWRegister(reg);
    return 0;
}