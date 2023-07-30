#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <stdbool.h>
#include <omp.h>
#include "WFSnapshot.h"

/* These structs should to match the definition in benchmark.py
 */
struct bench_result {
    float scan_throughput;
    float update_throughput;
    float mixed_throughput;
    float avg_indep_scan_latency;
    float avg_conc_scan_latency;
    float avg_update_latency;
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

double update_bench(WFSnapshot* benchSnapshot, int tid) {

    long lastThreadStamp = 0;
    //printf("Thread %d started writing.\n", tid);

    //#pragma omp barrier

    double  first = omp_get_wtime();
    update(benchSnapshot, tid, &lastThreadStamp);
    double second = omp_get_wtime();

    return second - first;
}

double scan_bench(WFSnapshot* benchSnapshot, int t) {

    int snap[t];
    //printf("Thread %d started scanning.\n", tid);

    //#pragma omp barrier

    double first = omp_get_wtime();
    scan(benchSnapshot,snap);
    double second = omp_get_wtime();

    return second - first;
}

struct bench_result small_bench(int t) {
    struct bench_result result;
    double tic, toc;

    int len = ( t < 16) ? t : 16;

    WFSnapshot benchSnapshot;
    createWFSnapshot(&benchSnapshot,t,1);

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
   
    // --------------------- Mixed section -------------------------
    
    if(t == 1){
    printf("Benchmarks for %d threads:\n * update thr = %f \n * update latency = %f \n * scan thr = %f \n * scan latency = %f\n",t,result.update_throughput,result.avg_update_latency,result.scan_throughput,result.avg_indep_scan_latency);
    return result;
    }
    
    len = t / 2;
    double timeArray3[len];
    tic = omp_get_wtime();
#pragma omp parallel default(none) shared(benchSnapshot, t, timeArray3, len)
    {
	int tid = omp_get_thread_num();
    	if(tid >= len){
    		long lastThreadStamp = 0;
		for(int i = 0; i < 1000; i++){
    			update(&benchSnapshot, tid, &lastThreadStamp);
		} 
	} else {
		timeArray3[tid]= scan_bench(&benchSnapshot, t);
		}
    }
    toc = omp_get_wtime();
    time_taken = toc - tic;
    result.mixed_throughput = (1001 * len)/time_taken;
    result.avg_conc_scan_latency = computeAverage(timeArray3,len);

    printf("Benchmarks for %d threads:\n * update thr = %f \n * update latency = %f \n * scan thr = %f \n * scan latency = %f\n * mixed thr = %f \n * concurrent scan latency = %f\n",t,result.update_throughput,result.avg_update_latency,result.scan_throughput,result.avg_indep_scan_latency,result.mixed_throughput,result.avg_conc_scan_latency);

    /*
    result.read_time = (float) (toc - tic);
    result.min_read_time= findMinTime(timeArray2, len);
    result.max_read_time = findMaxTime(timeArray2, len);
    printf("Snapshot test: scanning %d registers took: %fs, with min time %fs and max time %fs\n",
           len, result.read_time, result.min_read_time, result.max_read_time);

    printf("With %d parallel updates and %d concurrent scans: %f updating throughput and %f scanning throughput\n",\
        t,
        len,
           (float) t/result.write_time,
           (float) len/result.read_time);
*/
    return result;
}

/* main is not relevant for benchmark.py but necessary when run alone for
 * testing.
 */
int main(int argc, char * argv[]) {
    (void) argc;
    (void) argv;
    double tic = omp_get_wtime();
    small_bench(1);
    small_bench(2);
    small_bench(4);
    small_bench(8);
    small_bench(16);
    small_bench(32);
   // small_bench(64);
    double toc = omp_get_wtime();
    printf("\nTime taken: %f\n",toc-tic);

}
