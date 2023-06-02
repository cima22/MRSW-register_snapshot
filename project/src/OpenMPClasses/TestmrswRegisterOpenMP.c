#include "mrswRegisterOpenMP.h"
#define NUM_THREADS 4
#define NUM_READERS 4
#define NUM_WRITES 10

int main() {
    AtomicMRSWRegister reg;
    int init = 0;
    createAtomicMRSWRegister(&reg, &init, NUM_READERS);

    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int threadNum = omp_get_thread_num();
        if (threadNum == 0) { // Let's say thread 0 is our writer
            for (int i = 0; i < NUM_WRITES; ++i) {
                int* new_value = malloc(sizeof(int));
                *new_value = i; // new value to write
                writeMRSW(&reg, new_value);
            }
        } else { // Other threads are readers
            for (int i = 0; i < NUM_WRITES; ++i) {
                void* value = readMRSW(&reg);
                printf("Thread %d read value %d\n", threadNum, *(int*)value);
            }
        }
    }

    // destroyAtomicMRSWRegister(&reg);

    return 0;
}