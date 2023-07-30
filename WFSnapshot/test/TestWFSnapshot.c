//
// Created by Gabriele on 15/06/2023.
//

#include "omp.h"
#include <stdio.h>
#include <assert.h>
#include "../src/WFSnapshot.h"
#include <time.h>

int main(){

    // Test 1
    printf("Test 1: Snapshot object made for one thread, initialized with 1 as init value for register\n \
     * Assertion 1: thread scans 1 \n \
     * Assertion 2: thread scans 2 after calling update(2)\n");

    WFSnapshot test1Snapshot;
    if(createWFSnapshot(&test1Snapshot, 1, 1) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    int firstScan;
    int secondScan;

    scan(&test1Snapshot, &firstScan);
    printf("\nFirst scan returns: [%d]\n",firstScan);
    long lastThreadStamp = 0;
    update(&test1Snapshot, 2, &lastThreadStamp);
    scan(&test1Snapshot, &secondScan);
    printf("Second scan returns: [%d]\n",secondScan);
    freeWFSnapshot(&test1Snapshot);

    assert(firstScan == 1 && secondScan == 2);
    printf("\nTest 1 passed.\n\n");

    // Test 2:
    printf("Test 2: Snapshot made for 2,4,8,16,32,64 threads, initialized with value 1 in the registers.\n \
     * Assertion 1: a scan performed after the initialization returns an array of 1s\n \
     * Assertion 2: Every thread writes its id in its register. A susequent scan returns an arrays with the threadIDs\n");

    for (int i = 2; i <= 64; i = i * 2) {
        printf("\nTest 2 with %d threads.\n",i);
        WFSnapshot test2Snapshot;
        if(createWFSnapshot(&test2Snapshot,i,1) == EXIT_FAILURE){
            return EXIT_FAILURE;
        }
        int snap1[i];
        int snap2[i];
        int expected1[i];
        int expected2[i];
        for (int j = 0; j < i; ++j) {
            expected1[j] = 1;
            expected2[j] = j;
        }

        scan(&test2Snapshot,snap1);
        printf("First scan returns: ");
        for (int j = 0; j < i; ++j) {
            printf("[%d] ",snap1[j]);
        }

#pragma omp parallel default(none) shared(test2Snapshot) num_threads(i)
        {
            long lastThreadStamp = 0;
            update(&test2Snapshot,omp_get_thread_num(),&lastThreadStamp);
        }
#pragma omp barrier
        scan(&test2Snapshot,snap2);
        printf("\nSecond scan returns: ");
        for (int j = 0; j < i; ++j) {
            printf("[%d] ",snap2[j]);
        }

        freeWFSnapshot(&test2Snapshot);

        assert(memcmp(snap1,expected1,sizeof(expected1)) == 0 && memcmp(snap2,expected2,sizeof(expected2)) == 0);
        printf("\nTest 2 with %d threads has passed.\n",i);
    }
    printf("\nTest 2 passed.\n\n");

    // Test 3:
    printf("Test 3: Snapshot made for 4,8,16,32,64 threads. Thread 0 scans, sleeps for 10 milliseconds, scans again;\n \
     the other threads updates their register with values from 0 to 1000.\n \
     * Assertion: the second scan must have greater values than the first scan, element-wise.\n");

    for (int i = 4; i <= 64; i = i * 2) {
        printf("\nTest 3 with %d threads.\n",i);
        WFSnapshot test3Snapshot;
        if(createWFSnapshot(&test3Snapshot,i,1) == EXIT_FAILURE){
            return EXIT_FAILURE;
        }
        int snap1[i];
        int snap2[i];

#pragma omp parallel default(none) shared(test3Snapshot,snap1,snap2) num_threads(i)
        {
            if(omp_get_thread_num() == 0){
                scan(&test3Snapshot,snap1);
                struct timespec remaining, request = { 0, 10 * (2 << 20) };
                nanosleep(&request, &remaining);
                scan(&test3Snapshot,snap2);
            }
            else{
                long lastThreadStamp = 0;
                for (int j = 2; j < 1000; ++j) {
                    update(&test3Snapshot,j,&lastThreadStamp);
                }
            }
        }
#pragma omp barrier
        printf("Snap 1: ");
        for (int j = 0; j < i; ++j) {
            printf("[%d] ", snap1[j]);
        }
        printf("\nSnap 2: ");
        for (int j = 0; j < i; ++j) {
            printf("[%d] ", snap2[j]);
        }
        for (int j = 0; j < i; ++j) {
            if(snap1[j] > snap2[j])
                assert(false);
        }
        printf("\nTest 3 with %d threads has passed.\n",i);
        freeWFSnapshot(&test3Snapshot);

    }
    printf("\nTest 3 passed.\n\n");

    // Test 4:
    printf("Test 4: Snapshot made for 16,32,64 threads. \n \
    Thread 0 scans, thread 1 scans, sleeps for 10 milliseconds, scans again, finally thread 0 scans again;\n \
    The other threads updates their register with values from 0 to 1000.\n \
    * Assertion: the second scan must have greater values than the first scan, element-wise.\n");

    for (int i = 16; i <= 64; i *= 2) {
        printf("\nTest 4 with %d threads.\n",i);
        WFSnapshot test4Snapshot;
        if(createWFSnapshot(&test4Snapshot,i,1) == EXIT_FAILURE){
            return EXIT_FAILURE;
        }
        int snap1[i];
        int snap2[i];
        int snap3[i];
        int snap4[i];
        bool thread0Done = false;
        bool thread1Done = false;

#pragma omp barrier
#pragma omp parallel default(none) shared(test4Snapshot,snap1,snap2,snap3,snap4,i,thread0Done,thread1Done) num_threads(i)
        {

            int me = omp_get_thread_num();
            if(me == 0){
                scan(&test4Snapshot,snap1);
                printf("Snap 1: ");
                for (int j = 0; j < i; ++j) {
                    printf("[%d] ", snap1[j]);
                }
                thread0Done = true;
                while(!thread1Done){}
                scan(&test4Snapshot,snap4);
                printf("\nSnap 4: ");
                for (int j = 0; j < i; ++j) {
                    printf("[%d] ", snap4[j]);
                }
            }
            if(me == 1){
                while(!thread0Done){}
                scan(&test4Snapshot,snap2);
                printf("\nSnap 2: ");
                for (int j = 0; j < i; ++j) {
                    printf("[%d] ", snap2[j]);
                }
                struct timespec remaining, request = { 0, 10 * (2 << 20) };
                nanosleep(&request, &remaining);
                scan(&test4Snapshot,snap3);
                printf("\nSnap 3: ");
                for (int j = 0; j < i; ++j) {
                    printf("[%d] ", snap3[j]);
                }
                thread1Done = true;
            }
            if(me != 0 && me != 1){
                for (int j = 2; j < 1000; ++j) {
                    long lastThreadStamp = 0;
                    update(&test4Snapshot,j,&lastThreadStamp);
                }
            }
        }
#pragma omp barrier
        freeWFSnapshot(&test4Snapshot);
    }

    printf("\nTest 4 passed.\n\n");

    return 0;
}
