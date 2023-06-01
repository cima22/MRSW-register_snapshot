// In the SRC folder, just run make && ./main
#include "srswRegisterOpenMP.h"
#include <stdio.h>
#include<omp.h>
#include <time.h>
// The sleep is neccessary, else all the read operations are performed before the write ones
// may be because of the scheduler
// at first sight it seems linearizable; further testing needed
void reader(AtomicSRSWRegister* reg) {
    for (int i = 0; i < 20; i++) {
        int* value = (int*)read(reg);
        printf("Reader thread: Read value %d\n", *value);
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 10 * 1000 * 1000;  // 10,000 microseconds = 10 milliseconds
        nanosleep(&ts, NULL);
    }
}

void writer(AtomicSRSWRegister* reg) {
    for (int i = 0; i < 20; i++) {
        int new_value = i;
        write(reg, &new_value);
        printf("Writer thread: Wrote value %d\n", new_value);
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 10 * 1000 * 1000;  // 10,000 microseconds = 10 milliseconds
        nanosleep(&ts, NULL);
    }
}

int main() {
    int initial_value = 0;
    AtomicSRSWRegister* reg = createAtomicSRSWRegister(&initial_value);
    
    #pragma omp parallel num_threads(2)
    {
        if (omp_get_thread_num() == 0) {
            reader(reg);
        } else {
            writer(reg);
        }
    }

    int* last_value = (int*)read(reg);
    printf("Last read value: %d\n", *last_value);
    // not yet destroying, no freeing memory baby
    //destroyAtomicSRSWRegister(reg);
    return 0;
}