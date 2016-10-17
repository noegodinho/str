#include <time.h>
#include <pthread.h>

#define MAX_THREADS 3
#define BILLION 1e9

struct thread_info{
    pthread_t tid;
    int priority;
    struct timespec start;
    struct timespec end;
    struct timespec period;
}thread_info[MAX_THREADS];

int new_rt_task_make_periodic(int i, int priority, struct timespec start_time, struct timespec period, int end_time){
    thread_info[i].priority = priority;
    thread_info[i].start = start_time;
    thread_info[i].end.tv_sec = thread_info[i].start.tv_sec + end_time;
    thread_info[i].end.tv_nsec = thread_info[i].start.tv_nsec;
    thread_info[i].period = period;
}

int new_rt_task_make_periodic_relative_ns(int i, int priority, struct timespec start_delay, struct timespec period, int end_time){
    struct timespec actual_time;
    clock_gettime(CLOCK_MONOTONIC, &actual_time);

    thread_info[i].priority = priority;
    thread_info[i].start.tv_sec = actual_time.tv_sec + start_delay.tv_sec;
    thread_info[i].start.tv_nsec = actual_time.tv_nsec + start_delay.tv_nsec;

    if(thread_info[i].start.tv_nsec > BILLION){
        thread_info[i].start.tv_nsec -= BILLION;
        ++thread_info[i].start.tv_sec;
    }

    thread_info[i].end.tv_sec = thread_info[i].start.tv_sec + end_time;
    thread_info[i].end.tv_nsec = thread_info[i].start.tv_nsec;
    thread_info[i].period = period;
}

void new_rt_task_wait_period(){
    int i;
    pthread_t tid = pthread_self();

    for(i = 0; i < MAX_THREADS; ++i){
        if(thread_info[i].tid == tid){
            clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &thread_info[i].start, NULL);

            thread_info[i].start.tv_sec += period.tv_sec;
            thread_info[i].start.tv_nsec += period.tv_nsec;

            if(thread_info[i].start.tv_nsec > BILLION){
                thread_info[i].start.tv_nsec -= BILLION;
                ++thread_info[i].start.tv_sec;
            }

            break;
        }
    }
}
