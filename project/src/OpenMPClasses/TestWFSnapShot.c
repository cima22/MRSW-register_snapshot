#include <stdio.h>
#include <stdlib.h>
#include "WFSnapshot.h"
#include "omp.h"
#include <unistd.h>

#ifndef NUM_THREADS
#define NUM_THREADS 2
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
        update(&snapshot,NUM_THREADS);
    }

    scan(&snapshot,snap);
    for (int i = 0; i < NUM_THREADS; ++i) {
        printf("[%d] ",snap[i]);
    }
    // freeWFSnapshot(&snapshot);
    // free(snap);
    return 0;
}