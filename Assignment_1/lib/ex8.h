/****************************************************************
 * Autor: José Manuel C. Noronha
 * Autor: Noé Godinho
 * Turma: PL2
 * Grupo: 5
 * Ano Lectivo: 2016 - 2017
 ***************************************************************/

/* Includes, Definições e Notas */
#include <pthread.h>
#include <time.h>
#define BILLION 1e9
#define MAX_THREADS 3

/*
 * estrutura de criação de threads
 * variável com id da thread
 * a prioridade atribuida
 * o tempo de início da thread
 * o tempo de fim da thread
 * o período da thread
 */
struct thread_info{
    int priority;
    struct timespec start;
    struct timespec end;
    struct timespec period;
}thread_info[MAX_THREADS];

void new_rt_task_make_periodic(int, int, struct timespec, struct timespec, int);
void new_rt_task_make_periodic_relative_ns(int, int, struct timespec, struct timespec, int);
void new_rt_task_wait_period();
