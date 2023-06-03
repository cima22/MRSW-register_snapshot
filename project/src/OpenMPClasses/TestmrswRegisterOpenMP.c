#include "mrswRegisterOpenMP.h"
#define NUM_THREADS 2
#define NUM_READERS 1
#define NUM_WRITES 10

int main() {
    AtomicMRSWRegister* reg = (AtomicMRSWRegister*)malloc(sizeof(AtomicMRSWRegister));
    int init = 0;
                printf("got here");
    createAtomicMRSWRegister(reg, &init, NUM_READERS);
    // int sizeOfTable = reg.sizeOfTable;
    // for (int i = 0; i < (int) sizeOfTable; i++) {
    //     // testing, the lastRead[] is NULL
    //             for(int j = 0; j < NUM_READERS; j++){
    //             if(reg->a_table[i][me]->lastRead[j] == NULL)
    //                 printf("\n\nriginalplmmm\n\n");}

    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int threadNum = omp_get_thread_num();
        if (threadNum == 1) { // Let's say thread 0 is our writer
            for (int i = 0; i < NUM_WRITES; ++i) {
                int* new_value = malloc(sizeof(int));
                *new_value = i; // new value to write
                writeMRSW(reg, new_value);

            }
        } else { // Other threads are readers
                                TestingMRSWRegisterMemoryAllocated(reg);
            for (int i = 0; i < NUM_WRITES; ++i) {
                void* value = readMRSW(reg);
                printf("Thread %d read value %d\n", threadNum, *(int*)value);
            }
        }
    }

    // destroyAtomicMRSWRegister(&reg);

    return 0;
}