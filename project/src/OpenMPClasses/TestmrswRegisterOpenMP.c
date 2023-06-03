#include "mrswRegisterOpenMP.h"
#define NUM_THREADS 4
#define NUM_READERS 3
#define NUM_WRITES 10

int main() {
    AtomicMRSWRegister* reg = (AtomicMRSWRegister*)calloc(1, sizeof(AtomicMRSWRegister));
    int init = 0;
    
    createAtomicMRSWRegister(reg, init, NUM_READERS);
    #pragma omp parallel num_threads(NUM_THREADS)
    {   long ThreadLastStamp = 0;
        int threadNum = omp_get_thread_num();
        if (threadNum == NUM_THREADS - 1) { // Last thread has to be the writer in this code
            for (int i = 0; i < NUM_WRITES; ++i) {
                writeMRSW(reg,ThreadLastStamp, i);
                printf("Thread %d wrote value %d\n", threadNum, i);
                struct timespec ts;
                ts.tv_sec = 0;
                ts.tv_nsec = 10 * 1000 * 1000;  // 100,00 microseconds = 10 millisecond
                nanosleep(&ts, NULL);
            }
        } else { // Other threads are readers
            for (int i = 0; i < NUM_WRITES; ++i) {
                int value = readMRSW(reg,ThreadLastStamp);
                printf("Thread %d read value %d\n", threadNum, value);
                struct timespec ts;
                ts.tv_sec = 0;
                ts.tv_nsec = 10 * 1000 * 1000;  // 100,00 microseconds = 10 millisecond
                nanosleep(&ts, NULL);
            }
        }
    }

    return 0;
}