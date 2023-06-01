#include<omp.h>
#include<stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

static long num_steps = 100000;
double step;
#define NUMTHREADS 10

int main() {
    int i;
    double pi, sum = 0.0;
    step = 1.0/(double) num_steps;
    
    double inceput = omp_get_wtime();
    // omp_set_schedule(omp_sched_static, 0);
    // omp_set_schedule(omp_sched_dynamic, 0);
    omp_set_schedule(omp_sched_auto, 0);

    struct timespec ts;
    ts.tv_sec = 2;
    ts.tv_nsec = 1L;  // 1 nanosecond

    #pragma omp parallel num_threads(NUMTHREADS) reduction(+:sum)
    {
        int me = omp_get_thread_num();
        if(me < NUMTHREADS / 2)
            nanosleep(&ts, NULL);
        double x;
        #pragma omp for 
        for(i = 0; i < num_steps; i++) {
            x = (i + 0.5)*step;
            sum = sum + 4.0/(1.0 + x*x);
        }
    }

    double final = omp_get_wtime();
    printf("%f", (double)(final - inceput));
    
    pi = step * sum;
    printf("\n%f\n",pi);

    omp_sched_t kind;
    int modifier;
    omp_get_schedule(&kind, &modifier);

    if (kind == omp_sched_auto) {
        printf("Schedule is auto.\n");
    } else {
        printf("Schedule is not auto.\n");
    }

    return 0;
}
