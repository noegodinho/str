/****************************************************************
 * Autor: José Manuel C. Noronha
 * Autor: Noé Godinho
 * Turma: PL2
 * Grupo: 5
 * Ano Lectivo: 2016 - 2017
 ***************************************************************/

 /* Includes, Definições e Notas */
 #include "../lib/ex8.h"

/*
 * função que atribui os valores necessários
 * para tornar a thread i periódica
 */
void new_rt_task_make_periodic(int i, int priority, struct timespec start_time, struct timespec periodo, int end_time){
    thread_info[i].priority = priority;
    thread_info[i].start = start_time;
    /* aplica o tempo de início mais o tempo que a thread decorre */
    thread_info[i].end.tv_sec = thread_info[i].start.tv_sec + end_time;
    thread_info[i].end.tv_nsec = thread_info[i].start.tv_nsec;
    thread_info[i].period = periodo;
}

/*
 * igual à função anterior, no entanto aplica um delay de início
 */
void new_rt_task_make_periodic_relative_ns(int i, int priority, struct timespec start_delay, struct timespec periodo, int end_time){
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
    thread_info[i].period = periodo;
}

/*
 * função que faz parar a thread durante um determinado período
 */
void new_rt_task_wait_period(int i){
    /* faz sleep à thread até voltar a ser chamada novamente */
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &thread_info[i].start, NULL);

    /* adiciona o periodo à thread */
    thread_info[i].start.tv_sec += thread_info[i].period.tv_sec;
    thread_info[i].start.tv_nsec += thread_info[i].period.tv_nsec;

    /* condição para evitar overflow na variável de nanosegundos */
    if(thread_info[i].start.tv_nsec > BILLION){
        thread_info[i].start.tv_nsec -= BILLION;
        ++thread_info[i].start.tv_sec;
    }
}
