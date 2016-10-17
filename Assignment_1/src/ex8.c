#include <time.h>
#include <pthread.h>

struct thread_info{
    pthread_t tid;
    int priority;
    struct timespec start;
    struct timespec end;
    struct timespec period;
};

int new_rt_task_make_periodic(struct thread_info t_info){

}

int new_rt_task_make_periodic_relative_ns(struct thread_info t_info){

}

void new_rt_task_wait_period(){
    
}
