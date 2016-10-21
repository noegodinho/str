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
#define BILLION 1e9

/* PL2, G5 */

void *func1(void *arg);
void *func2(void *arg);
void *func3(void *arg);
void priorities(int priority_number);

int main(){
    cpu_set_t set;
    pthread_t thread_id[NUM_THREADS];
    int i;

    /* limpa a máscara do CPU e define a utilização do CPU 0 */
    CPU_ZERO(&set);    
    CPU_SET(0, &set);

    /* restringe a execução ao CPU 0 */
    if(sched_setaffinity(0, sizeof(cpu_set_t), &set) == -1){
        perror("Error from sched_setaffinity");
    }

    /* bloqueia a possibilidade de haver troca de memória para a swap */
    if(mlockall(MCL_CURRENT | MCL_FUTURE) != 0){
        perror("Error from mlockall");
    }

    printf("\n");

    /* cria as threads */
    pthread_create(&thread_id[0], NULL, &func1, NULL);
    pthread_create(&thread_id[1], NULL, &func2, NULL);
    pthread_create(&thread_id[2], NULL, &func3, NULL);

    /* espera a que as threads terminem */
    for(i = 0; i < NUM_THREADS; ++i){
        pthread_join(thread_id[i], NULL);
    }

    return 0;
}

void *func1(void *arg){
    /* define a prioridade da thread */
    priorities(99);
    struct timespec start, end;
    uint64_t average = 0;
    double final_average;

    /* 
     * ciclo para fazer um determinado nº de execuções
     * e fazer várias execuções por função
     * para obter uma média do tempo de execução
     */
    for(int i = 0; i < NUM_EXEC; ++i){
        /* início da execução */
        clock_gettime(CLOCK_MONOTONIC, &start);
        f1(2, 5);
        /* final da execução */
        clock_gettime(CLOCK_MONOTONIC, &end);

        /* 
         * soma-se o valor obtido para o cálculo posterior da média
         * convertem-se os segundos obtidos para nanosegundos (BILLION = 1e9)
         * para poder realizar a sua soma
         */
        average += (BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec);
    }

    /* converte a soma de nanosegundos obtidos para segundos */
    final_average = (double)average / BILLION;

    /* faz a média dos segundos com o nº de execuções e imprime o resultado */
    printf("Time f1: %f\n", final_average / NUM_EXEC);
    /* faz exit da thread */
    pthread_exit(NULL);
}

void *func2(void *arg){
    /* define a prioridade da thread */
    priorities(99);
    struct timespec start, end;
    uint64_t average = 0;
    double final_average;

    /* 
     * ciclo para fazer um determinado nº de execuções
     * e fazer várias execuções por função
     * para obter uma média do tempo de execução
     */
    for(int i = 0; i < NUM_EXEC; ++i){
        /* início da execução */
        clock_gettime(CLOCK_MONOTONIC, &start);
        f2(2, 5);
        /* final da execução */
        clock_gettime(CLOCK_MONOTONIC, &end);

        /* 
         * soma-se o valor obtido para o cálculo posterior da média
         * convertem-se os segundos obtidos para nanosegundos (BILLION = 1e9)
         * para poder realizar a sua soma
         */
        average += (BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec);
    }

    /* converte a soma de nanosegundos obtidos para segundos */
    final_average = (double)average / BILLION;

    /* faz a média dos segundos com o nº de execuções e imprime o resultado */
    printf("Time f2: %f\n", final_average / NUM_EXEC);
    /* faz exit da thread */
    pthread_exit(NULL);
}

void *func3(void *arg){
    /* define a prioridade da thread */
    priorities(99);
    struct timespec start, end;
    uint64_t average = 0;
    double final_average;

    /* 
     * ciclo para fazer um determinado nº de execuções
     * e fazer várias execuções por função
     * para obter uma média do tempo de execução
     */
    for(int i = 0; i < NUM_EXEC; ++i){
        /* início da execução */
        clock_gettime(CLOCK_MONOTONIC, &start);
        f3(2, 5);
        /* final da execução */
        clock_gettime(CLOCK_MONOTONIC, &end);

        /* 
         * soma-se o valor obtido para o cálculo posterior da média
         * convertem-se os segundos obtidos para nanosegundos (BILLION = 1e9)
         * para poder realizar a sua soma
         */
        average += (BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec);
    }

    /* converte a soma de nanosegundos obtidos para segundos */
    final_average = (double)average / BILLION;

    /* faz a média dos segundos com o nº de execuções e imprime o resultado */
    printf("Time f3: %f\n", final_average / NUM_EXEC);
    /* faz exit da thread */
    pthread_exit(NULL);
}

void priorities(int priority_number){
    /* vai buscar o id da thread */
    pthread_t id = pthread_self();
    struct sched_param param;

    /* define o valor da prioridade recebido */
    param.sched_priority = priority_number;

    /* aplica um escalonamento RMPO */
    if(pthread_setschedparam(id, SCHED_FIFO, &param) != 0){
        perror("Error from pthread_setschedparam");
    }
}
