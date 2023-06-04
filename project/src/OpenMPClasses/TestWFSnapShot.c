#include <stdio.h>
#include <stdlib.h>
#include "WFSnapshot.h"
#include "omp.h"
#include <unistd.h>

#ifndef NUM_THREADS
#define NUM_THREADS 5
#endif

int main() {
    WFSnapshot snapshot;
    if(createWFSnapshot(&snapshot,NUM_THREADS, 0) == EXIT_FAILURE)
        return EXIT_FAILURE;
    int* snap = calloc(NUM_THREADS, sizeof(int));
    if(snap == NULL){
        fprintf(stderr,"Memory allocation failed: snap in main");
        return EXIT_FAILURE;
    }

#pragma omp parallel default(none) shared(snapshot,snap) num_threads(NUM_THREADS)
    {
        long* threadLastStamp = (long*)calloc(NUM_THREADS,sizeof(long));
        int thr = omp_get_thread_num();
        update(&snapshot,thr + 4, threadLastStamp);
    }

    scan(&snapshot,snap);
    for (int i = 0; i < NUM_THREADS; ++i) {
        printf("[%d] ",snap[i]);
    }
    // freeWFSnapshot(&snapshot);
    // free(snap);
    return 0;
}