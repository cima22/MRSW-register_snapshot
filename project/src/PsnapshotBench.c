#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <stdbool.h>
#include <omp.h>
// #include "WFSnapshot.h"
#include "PSnapshot.h"

struct bench_result {
    float scan_throughput;
    float update_throughput;
    float mult_update_throughput;
    float mixed_throughput;
    float avg_indep_scan_latency;
    float avg_mixed_latency;
    float avg_update_latency;
    float avg_mult_update_latency;
};

float computeAverage(double * timeArray, int arraySize){
	float sum = 0;
	for(int i = 0; i < arraySize; i++){
		sum += timeArray[i];
	}
	return sum / (double) arraySize;
}

float findMinTime(float* timeArray, int arraySize){
    if (arraySize <= 0) {
        return -1;
    }

    float min = timeArray [0];
    for (int i = 0; i < arraySize; ++i) {
        if ( timeArray [i] < min ) {
            min = timeArray [i];
        }
    }
    return min;
}

float findMaxTime(float* timeArray, int arraySize){
    if (arraySize <= 0) {
        return -1;
    }

    float max = timeArray [0];
    for (int i = 0; i < arraySize; ++i) {
        if ( timeArray [i] > max ) {
            max = timeArray [i];
        }
    }
    return max;
}

double update_bench(PSnapshot* benchSnapshot, int tid) {

    //printf("Thread %d started writing.\n", tid);

    // #pragma omp barrier

    double  first = omp_get_wtime();
    update(benchSnapshot, tid, tid, tid);
    double second = omp_get_wtime();

    return second - first;
}
double update_bench_mult_reg_writes(PSnapshot* benchSnapshot, int tid, int* registers, int nr_reg) {
    // #pragma omp barrier

    double  first = omp_get_wtime();
    for(int i = 0;i<nr_reg;i++) {
        update(benchSnapshot, registers[i], tid, tid);
    }
    double second = omp_get_wtime();

    return second - first;
}

double scan_bench(PSnapshot* benchSnapshot, int t) {

    int snap[t];
    //printf("Thread %d started scanning.\n", tid);
    int registers[t];
    for(int i = 0;i<t;i++)registers[i] = i;
    // #pragma omp barrier

    double first = omp_get_wtime();
    p_snapshot(benchSnapshot,snap,registers , t);
    double second = omp_get_wtime();

    return second - first;
}

struct bench_result small_bench(int t) {
    struct bench_result result;
    double tic, toc;

    int len = ( t < 16) ? t : 16;

    PSnapshot benchSnapshot;
    createPSnapshot(&benchSnapshot,t,t,1);

    omp_set_num_threads(t);

    // --------------------- Updating section ---------------------

    tic = omp_get_wtime();
    double timeArray[t];

    #pragma omp parallel default(none) shared(benchSnapshot, t, timeArray)
	{
            int tid = omp_get_thread_num();
                timeArray[tid]= update_bench(&benchSnapshot, tid);
    }

    toc = omp_get_wtime();
    double time_taken = toc - tic;
    result.update_throughput =  t / time_taken;
    result.avg_update_latency = computeAverage(timeArray,t);
    /*
    result.write_time = (float) (toc - tic);
    result.min_write_time = findMinTime(timeArray, t);
    result.max_write_time = findMaxTime(timeArray, t);
    printf("Snapshot test: updating %d registers took: %fs, with min time %fs and max time %fs\n",
           t, result.write_time, result.min_write_time, result.max_write_time);
*/
    #pragma omp barrier

    // ------------------- Multiple Updates section -------------------

    tic = omp_get_wtime();
    double timeArray3[t];

    #pragma omp parallel default(none) shared(benchSnapshot, t, timeArray3)
	{
            int tid = omp_get_thread_num();
            int registers[t];
            for(int i = 0;i<t;i++)registers[i] = i;

            timeArray3[tid]= update_bench_mult_reg_writes(&benchSnapshot, tid, registers, t);
    }

    toc = omp_get_wtime();
    time_taken = toc - tic;
    result.mult_update_throughput =  (double)t*(double)t / time_taken;
    result.avg_mult_update_latency = computeAverage(timeArray3,t) / (double)t;
    /*
    result.write_time = (float) (toc - tic);
    result.min_write_time = findMinTime(timeArray, t);
    result.max_write_time = findMaxTime(timeArray, t);
    printf("Snapshot test: updating %d registers took: %fs, with min time %fs and max time %fs\n",
           t, result.write_time, result.min_write_time, result.max_write_time);
*/
    #pragma omp barrier





    // --------------------- Scanning section ---------------------


    tic = omp_get_wtime();
    double timeArray2[t];
   
    #pragma omp parallel default(none) shared(benchSnapshot, t, timeArray2)
    {
	    int tid = omp_get_thread_num();
	    timeArray2[tid]= scan_bench(&benchSnapshot, t);
    }

    toc = omp_get_wtime();
    time_taken = toc - tic;
    result.scan_throughput = t / time_taken;
    result.avg_indep_scan_latency = computeAverage(timeArray2,t);

    // --------------------- Mixed section ---------------------

    tic = omp_get_wtime();
    double timeArray4[t];
    len = t/2;
    tic = omp_get_wtime();

    #pragma omp parallel default(none) shared(benchSnapshot, t, timeArray4, len)
	{
            int tid = omp_get_thread_num();
            if(tid>=len) {
                for(int i = 0;i<100000;i++) {
                    update_bench(&benchSnapshot, tid);
                }
            }
            else {
                timeArray4[tid] = scan_bench(&benchSnapshot,t);
            }
    }

    toc = omp_get_wtime();
    time_taken = toc - tic;
    result.mixed_throughput =  (100001*len) / time_taken;
    result.avg_mixed_latency = computeAverage(timeArray4,t);

    #pragma omp barrier
   
    printf("Tests for %d threads:\n", t);
    printf(" * update thr = %f \n", result.update_throughput);
    printf(" * update latency = %f \n", result.avg_update_latency);
    printf(" * Mult_update thr = %f \n", result.mult_update_throughput);
    printf(" * Mult_update latency = %f \n", result.avg_mult_update_latency);
    printf(" * scan thr = %f \n", result.scan_throughput);
    printf(" * scan latency = %f\n", result.avg_indep_scan_latency);
    printf(" * mixed thr = %f \n", result.mixed_throughput);
    printf(" * mixed latency = %f\n", result.avg_mixed_latency);

    return result;
}

/* main is not relevant for benchmark.py but necessary when run alone for
 * testing.
 */
int main(int argc, char * argv[]) {
    double tic, toc;
    tic = omp_get_wtime();
    (void) argc;
    (void) argv;
    small_bench(1);
    small_bench(2);
    small_bench(4);
    small_bench(8);
    small_bench(16);
    small_bench(32);
    small_bench(64);
    toc = omp_get_wtime();
    printf("\nTime taken: %f\n", toc - tic);

}