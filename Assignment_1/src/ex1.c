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

void *func1(void *arg);
void *func2(void *arg);
void *func3(void *arg);
void priorities(int priority_number);

int main(){
    cpu_set_t set;
    pthread_t thread_id[NUM_THREADS];
    int i;

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

    pthread_create(&thread_id[0], NULL, &func1, NULL);
    pthread_create(&thread_id[1], NULL, &func2, NULL);
    pthread_create(&thread_id[2], NULL, &func3, NULL);

    for(i = 0; i < NUM_THREADS; ++i){
        pthread_join(thread_id[i], NULL);
    }

    return 0;
}

void *func1(void *arg){
    priorities(99);
    struct timespec start, end;
    uint64_t average = 0;
    double final_average;

    for(int i = 0; i < NUM_EXEC; ++i){
        clock_gettime(CLOCK_MONOTONIC, &start);
        f1(2, 5);
        clock_gettime(CLOCK_MONOTONIC, &end);

        average += (BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec);
    }

    final_average = (double)average / BILLION;

    printf("Time f1: %f\n", final_average / NUM_EXEC);
    pthread_exit(NULL);
}

void *func2(void *arg){
    priorities(99);
    struct timespec start, end;
    uint64_t average = 0;
    double final_average;

    for(int i = 0; i < NUM_EXEC; ++i){
        clock_gettime(CLOCK_MONOTONIC, &start);
        f2(2, 5);
        clock_gettime(CLOCK_MONOTONIC, &end);

        average += (BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec);
    }

    final_average = (double)average / BILLION;

    printf("Time f2: %f\n", final_average / NUM_EXEC);
    pthread_exit(NULL);
}

void *func3(void *arg){
    priorities(99);
    struct timespec start, end;
    uint64_t average = 0;
    double final_average;

    for(int i = 0; i < NUM_EXEC; ++i){
        clock_gettime(CLOCK_MONOTONIC, &start);
        f3(2, 5);
        clock_gettime(CLOCK_MONOTONIC, &end);

        average += (BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec);
    }

    final_average = (double)average / BILLION;

    printf("Time f3: %f\n", final_average / NUM_EXEC);
    pthread_exit(NULL);
}

void priorities(int priority_number){
    pthread_t id = pthread_self();
    struct sched_param param;

    param.sched_priority = priority_number;

    if(pthread_setschedparam(id, SCHED_FIFO, &param) != 0){
        perror("Error from pthread_setschedparam");
    }
}
