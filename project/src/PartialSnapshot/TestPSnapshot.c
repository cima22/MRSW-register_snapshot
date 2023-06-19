#include "omp.h"
#include <stdio.h>
#include <assert.h>
#include "PSnapshot.h"
#include <time.h>

#define MAXTHRD 64

int main(){

    // Test 1
    printf("Test 1: Snapshot object made for one thread, initialized with 1 as init value for register\n \
     * Assertion 1: thread scans 1 \n \
     * Assertion 2: thread scans 2 after calling update(2)\n");

    int init = 1;
    PSnapshot test1Snapshot;
    if(createPSnapshot(&test1Snapshot, 1, 2, init) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    int registerSet1[1] = {0}; // array containing 0
    int* pSnap1 = calloc(1, sizeof(int));
    if(p_snapshot(&test1Snapshot, pSnap1, registerSet1, 1) == EXIT_FAILURE)
        return EXIT_FAILURE;
    int firstScan = pSnap1[0];

    printf("\nFirst scan returns: [%d]\n",firstScan);
    update(&test1Snapshot, 0, 2, 0);

    int registerSet2[1] = {0}; // array containing 0
    int* pSnap2 = calloc(1, sizeof(int));
    if(p_snapshot(&test1Snapshot, pSnap2, registerSet2, 1) == EXIT_FAILURE)
        return EXIT_FAILURE;
    int secondScan = pSnap2[0];

    printf("Second scan returns: [%d]\n",secondScan);
    // freePSnapshot(&test1Snapshot);

    assert(firstScan == 1 && secondScan == 2);
    printf("\nTest 1 passed.\n\n");

    free(pSnap1);
    free(pSnap2);
    freePSnapshot(&test1Snapshot);


    // Test 2:
    printf("Test 2: Snapshot made for 2,4,8,16,32,64 threads, initialized with value 1 in the registers.\n \
     * Assertion 1: a scan performed after the initialization returns an array of 1s\n \
     * Assertion 2: Every thread writes its id in its register. A susequent scan returns an arrays with the threadIDs\n");

    for (int i = 2; i <= MAXTHRD; i = i * 2) {
        printf("\nTest 2 with %d threads.\n",i);
        PSnapshot test2Snapshot;
        if(createPSnapshot(&test2Snapshot, i, i, 1) == EXIT_FAILURE){
            return EXIT_FAILURE;
        }
        int snap1[i];
        int snap2[i];
        int expected1[i];
        int expected2[i];
        int registers[i];
        for (int j = 0; j < i; ++j) {
            expected1[j] = 1;
            expected2[j] = j;
            registers[j] = j;
        }

        p_snapshot(&test2Snapshot,snap1, registers, i);
        printf("First scan returns: ");
        for (int j = 0; j < i; ++j) {
            printf("[%d] ",snap1[j]);
        }
        // int maximum_threadNR = 0;
#pragma omp parallel default(none) shared(test2Snapshot) num_threads(i)
        {
            // testing to see how many trhreads on a machine
            // if(maximum_threadNR < omp_get_thread_num()){maximum_threadNR = omp_get_thread_num();printf(" %d\n", maximum_threadNR); }

            // long lastThreadStamp = 0;
            update(&test2Snapshot,omp_get_thread_num(),omp_get_thread_num(), omp_get_thread_num());
        }
#pragma omp barrier
        p_snapshot(&test2Snapshot,snap2, registers, i);
        printf("\nSecond scan returns: ");
        for (int j = 0; j < i; ++j) {
            printf("[%d] ",snap2[j]);
        }

        freePSnapshot(&test2Snapshot);

        assert(memcmp(snap1,expected1,sizeof(expected1)) == 0 && memcmp(snap2,expected2,sizeof(expected2)) == 0);
        printf("\nTest 2 with %d threads has passed.\n",i);
    }
    printf("\nTest 2 passed.\n\n");


    // Test 3:
    printf("Test 3: Snapshot made for 2,4,8,16,32,64 threads. Thread 0 scans, sleeps for 10 milliseconds, scans again;\n \
     the other threads updates their register with values from 0 to 1000.\n \
     * Assertion: the second scan must have greater values than the first scan, element-wise.\n");

    for (int i = 2; i <= MAXTHRD; i = i * 2) {
        printf("\nTest 3 with %d threads.\n",i);
        PSnapshot test3Snapshot;
        if(createPSnapshot(&test3Snapshot, i, i,0) == EXIT_FAILURE){
            return EXIT_FAILURE;
        }
        int snap1[i];
        int snap2[i];
        int registers[i];
        for (int j = 0; j < i; ++j) {
            registers[j] = j;
        }
#pragma omp parallel default(none) shared(test3Snapshot,snap1,snap2, registers) num_threads(i)
        {
            if(omp_get_thread_num() == 0){
                struct timespec remaining, request = { 0, 1 * (2 << 20) };
                nanosleep(&request, &remaining);
                p_snapshot(&test3Snapshot,snap1, registers, omp_get_num_threads());
                struct timespec remainingp, requestp = { 0, 2 * (2 << 20) };
                nanosleep(&requestp, &remainingp);
                p_snapshot(&test3Snapshot,snap2, registers, omp_get_num_threads());
                // printf("avem %d:\n", snap1[0]);
            }
            else{
                // long lastThreadStamp = 0;
                for (int j = 0; j < 1000; ++j) {
                    update(&test3Snapshot,omp_get_thread_num(),j,omp_get_thread_num());
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
        freePSnapshot(&test3Snapshot);

    }
    printf("\nTest 3 passed.\n\n");

    // Test 4:
    printf("Test 4: Snapshot made for 4,8,12,..,64 threads. \n \
    Thread 0 scans, thread 1 scans, sleeps for 10 milliseconds, scans again, finally thread 0 scans again;\n \
    The other threads updates their register with values from 0 to 1000.\n \
    * Assertion: the second scan must have greater values than the first scan, element-wise.\n");

    for (int i = 4; i <= MAXTHRD/2; i *= 2) {
        printf("\nTest 4 with %d threads.\n",i);
        PSnapshot test4Snapshot;
        if(createPSnapshot(&test4Snapshot,i,i,0) == EXIT_FAILURE){
            return EXIT_FAILURE;
        }
        int snap1[i];
        int snap2[i];
        int snap3[i];
        int snap4[i];
        bool thread0Done = false;
        bool thread1Done = false;
        int registers[i];
        for(int j = 0;j<i;j++)registers[j] = j;

// #pragma omp barrier
#pragma omp parallel default(none) shared(test4Snapshot,snap1,snap2,snap3,snap4,i,thread0Done,thread1Done,registers) num_threads(i)
        {


            int me = omp_get_thread_num();
            if(me == 0){
                p_snapshot(&test4Snapshot,snap1,registers,i);
                printf("Snap 1: ");
                for (int j = 0; j < i; ++j) {
                    printf("[%d] ", snap1[j]);
                }
                thread0Done = true;
                while(!thread1Done){}
                p_snapshot(&test4Snapshot,snap4, registers,i);
                printf("\nSnap 4: ");
                for (int j = 0; j < i; ++j) {
                    printf("[%d] ", snap4[j]);
                }
            }
            if(me == 1){
                while(!thread0Done){}
                p_snapshot(&test4Snapshot,snap2, registers,i);
                printf("\nSnap 2: ");
                for (int j = 0; j < i; ++j) {
                    printf("[%d] ", snap2[j]);
                }
                struct timespec remaining, request = { 0, 10 * (2 << 20) };
                nanosleep(&request, &remaining);
                p_snapshot(&test4Snapshot,snap3, registers, i);
                printf("\nSnap 3: ");
                for (int j = 0; j < i; ++j) {
                    printf("[%d] ", snap3[j]);
                }
                thread1Done = true;
            }
            if(me != 0 && me != 1){
                for (int j = 0; j < 10000; ++j) {
                    update(&test4Snapshot,omp_get_thread_num(), j, omp_get_thread_num());
                }
            }
        }
#pragma omp barrier
        freePSnapshot(&test4Snapshot);
        for(int j = 0;j<i;j++) {
            if(snap1[j] > snap4[j]){ printf("\npozitie: %d  snap1:%d  snap4: %d \n",j, snap1[j], snap4[j]);
                assert(false);}
            if(snap2[j] > snap3[j]){ printf("\npozitie: %d  snap2:%d  snap3: %d \n",j, snap2[j], snap3[j]);
                assert(false);}
        }
    }

    printf("\nTest 4 passed.\n\n");


    // Test 5:
printf("Test 5: Snapshot made for 4,8,12,..,64 threads with randomly sized and valued registers.\n \
Thread 0 scans, thread 1 scans, sleeps for 10 milliseconds, scans again, finally thread 0 scans again.\n \
Both threads scan a random number of registers, and the scanned registers are random \n \
* Assertion: the second scan must have greater values than the first scan, element-wise.\n");

for (int i = 4; i <= MAXTHRD; i += 4) {
    printf("\nTest 5 with %d threads.\n",i);
    PSnapshot test5Snapshot;
    if(createPSnapshot(&test5Snapshot, i, i, 0) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    
    int snap1[i];
    int snap2[i];
    int snap3[i];
    int snap4[i];

    bool thread0Done = false;
    bool thread1Done = false;
    srand(time(NULL));
    int reg_size_read_1 = 0;
    int reg_size_read_2 = 0;

#pragma omp barrier
#pragma omp parallel default(none) shared(test5Snapshot,snap1,snap2,snap3,snap4,i,thread0Done,thread1Done,reg_size_read_1,reg_size_read_2) num_threads(i)
    {
        int me = omp_get_thread_num();
        int registerSize = rand() % (i-2) + 1; // random size for the vector
        if(omp_get_thread_num() == 0) reg_size_read_1 = registerSize;
        if(omp_get_thread_num() == 1) reg_size_read_2 = registerSize;
        int registers[registerSize];
        for(int j = 0; j < registerSize; j++){
            registers[j] = rand() % (i-2) + 2; // put in the vector a random register
        }
        // All the registers should have different values
        // It works for them having the same value as well, but it is not interesting
        for (int j = 0; j < registerSize; j++) {
            for (int k = j + 1; k < registerSize; k++) {
                while (registers[j] == registers[k]) {
                    registers[k] = rand() % (i - 2) + 2;
                }
            }
        }
    
        if(me == 0){
            struct timespec remaining1, request1 = { 0, 0.1 * (2 << 20) };
                nanosleep(&request1, &remaining1);
            p_snapshot(&test5Snapshot, snap1, registers, registerSize);
            printf("\nSnap 1: ");
            for (int j = 0; j < registerSize; ++j) {
                printf("(register %d: [%d]) ",registers[j], snap1[j]);
            }
            struct timespec remaining11, request11 = { 0, 0.3 * (2 << 20) };
            nanosleep(&request11, &remaining11);
            thread0Done = true;
            while(!thread1Done){}
            p_snapshot(&test5Snapshot, snap4, registers, registerSize);
            printf("\nSnap 4: ");
            for (int j = 0; j < registerSize; ++j) {
                printf("(register %d: [%d]) ",registers[j], snap4[j]);
            }
        }
        if(me == 1){
            struct timespec remaining2, request2 = { 0, 0.1 * (2 << 20) };
                nanosleep(&request2, &remaining2);
            while(!thread0Done){}
            p_snapshot(&test5Snapshot, snap2, registers, registerSize);
            printf("\nSnap 2: ");
            for (int j = 0; j < registerSize; ++j) {
                printf("(registru %d: [%d]) ",registers[j], snap2[j]);
            }
            struct timespec remaining22, request22 = { 0, 0.3 * (2 << 20) };
                nanosleep(&request22, &remaining22);
            p_snapshot(&test5Snapshot, snap3, registers, registerSize);
            printf("\nSnap 3: ");
            for (int j = 0; j < registerSize; ++j) {
                printf("(registru %d: [%d]) ",registers[j], snap3[j]);
            }
            thread1Done = true;
        }
        if(me != 0 && me != 1){
            for (int j = 0; j < 1000; ++j) {
                update(&test5Snapshot, omp_get_thread_num(), j, omp_get_thread_num());
            }
        }
    }
    #pragma omp barrier
    freePSnapshot(&test5Snapshot);

    for(int j = 0;j<reg_size_read_1;j++) {
    if(snap1[j] > snap4[j])
        assert(false);
    }
    for(int j = 0;j<reg_size_read_2;j++) {
    if(snap2[j] > snap3[j])
        assert(false);
    }

}

printf("\nTest 5 passed.\n\n");

    return 0;
}