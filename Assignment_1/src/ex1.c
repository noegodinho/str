#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <sys/mman.h>
#include "../lib/func.h"

#define NUM_THREADS 3
#define NUM_EXEC 100
#define BILLION 1000000000L

//#define DEBUG

/* PL2, G5 */

void *calculate_time(void *arg);

int main(){
    cpu_set_t set;
    pthread_t thread_id[NUM_THREADS];
    int i;
    int id[NUM_THREADS];

    /* clear cpu mask */
    CPU_ZERO(&set);
    /* set cpu 0 */
    CPU_SET(0, &set);

    /* 0 is the calling process */
    if(sched_setaffinity(0, sizeof(cpu_set_t), &set) == -1){
        perror("Error from sched_setaffinity");
    }

    /* needs root permissions */
    if(mlockall(MCL_CURRENT | MCL_FUTURE) != 0){
        perror("Error from mlockall");
    }

    printf("\n");

    for(i = 0; i < NUM_THREADS; ++i){
        id[i] = i;
        pthread_create(&thread_id[i], NULL, &calculate_time, &id[i]);
    }

    for(i = 0; i < NUM_THREADS; ++i){
        pthread_join(thread_id[i], NULL);
    }

    return 0;
}

void *calculate_time(void *arg){
    int value = *((int*)arg);

    #ifdef DEBUG
        printf("%d\n", value);
    #endif

    int i;
    uint64_t average = 0;
    double final_average;
    struct timespec start, end;

    for(i = 0; i < NUM_EXEC; ++i){
        switch(value){
            case 0:
                clock_gettime(CLOCK_REALTIME, &start);
                f1(2, 5);
                clock_gettime(CLOCK_REALTIME, &end);
                break;
            case 1:
                clock_gettime(CLOCK_REALTIME, &start);
                f2(2, 5);
                clock_gettime(CLOCK_REALTIME, &end);
                break;
            case 2:
                clock_gettime(CLOCK_REALTIME, &start);
                f3(2, 5);
                clock_gettime(CLOCK_REALTIME, &end);
                break;
        }

        average += (BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec);
    }

    /* convert nanoseconds to seconds */
    final_average = (double)average / BILLION;

    printf("Time f%d: %lf\n", value, final_average / NUM_EXEC);
    pthread_exit(NULL);
}
