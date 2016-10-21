#include <time.h>
#include <pthread.h>

#define MAX_THREADS 3
#define BILLION 1e9

/* 
 * estrutura de criação de threads
 * variável com id da thread
 * a prioridade atribuida
 * o tempo de início da thread
 * o tempo de fim da thread
 * o período da thread
 */
struct thread_info{
    pthread_t tid;
    int priority;
    struct timespec start;
    struct timespec end;
    struct timespec period;
}thread_info[MAX_THREADS];

/*
 * função que atribui os valores necessários
 * para tornar a thread i periódica
 */
int new_rt_task_make_periodic(int i, int priority, struct timespec start_time, struct timespec period, int end_time){
    thread_info[i].priority = priority;
    thread_info[i].start = start_time;
    /* aplica o tempo de início mais o tempo que a thread decorre */
    thread_info[i].end.tv_sec = thread_info[i].start.tv_sec + end_time;
    thread_info[i].end.tv_nsec = thread_info[i].start.tv_nsec;
    thread_info[i].period = period;
}

/* 
 * igual à função anterior, no entanto aplica um delay de início
 */
int new_rt_task_make_periodic_relative_ns(int i, int priority, struct timespec start_delay, struct timespec period, int end_time){
    struct timespec actual_time;
    /* estrutura para obter o tempo actual */
    clock_gettime(CLOCK_MONOTONIC, &actual_time);

    thread_info[i].priority = priority;
    /* como a thread inicia com um delay, é-lhe atribuído o tempo obtido mais o delay para início da thread */
    thread_info[i].start.tv_sec = actual_time.tv_sec + start_delay.tv_sec;
    thread_info[i].start.tv_nsec = actual_time.tv_nsec + start_delay.tv_nsec;

    /* condição para evitar overflow na variável de nanosegundos */
    if(thread_info[i].start.tv_nsec > BILLION){
        thread_info[i].start.tv_nsec -= BILLION;
        ++thread_info[i].start.tv_sec;
    }

    /* aplica o tempo de início mais o tempo que a thread decorre */
    thread_info[i].end.tv_sec = thread_info[i].start.tv_sec + end_time;
    thread_info[i].end.tv_nsec = thread_info[i].start.tv_nsec;
    thread_info[i].period = period;
}

/* 
 * função que faz parar a thread durante um determinado período
 */
void new_rt_task_wait_period(){
    int i;
    /* função para obter o id da thread */
    pthread_t tid = pthread_self();

    /* percorre o array de threads até encontrar a correspondente */
    for(i = 0; i < MAX_THREADS; ++i){
        if(thread_info[i].tid == tid){
            /* faz sleep à thread até voltar a ser chamada novamente */
            clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &thread_info[i].start, NULL);

            /* adiciona o periodo à thread */
            thread_info[i].start.tv_sec += period.tv_sec;
            thread_info[i].start.tv_nsec += period.tv_nsec;

            /* condição para evitar overflow na variável de nanosegundos */
            if(thread_info[i].start.tv_nsec > BILLION){
                thread_info[i].start.tv_nsec -= BILLION;
                ++thread_info[i].start.tv_sec;
            }

            break;
        }
    }
}
