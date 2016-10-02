
/* Nota: ms = milisegundos */

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
#define BILLION 1000000000L

/* Definição dos períodos de activação de cada tarefa em ms */
#define P1_Activacao 100
#define P2_Activacao 200
#define P3_Activacao 300

/* Estrutura com o tempo de execução das tarefas */
struct tempo_execucao{
	long int Inicio,Fim;
}relogio;


//#define DEBUG

/* PL2, G5 */

void *func1(void *arg);
void *func2(void *arg);
void *func3(void *arg);
void priorities(int);
long int hora_sistema_ms();
void sleep_thr(long int);

int main(){
	printf("===========================================================\n");
	printf("================== Iniciando o Programa! ==================\n");
	printf("=========== Sistemas de Tempo Real, PL2, Grupo5 ===========\n");
	printf("===========================================================\n\n");

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

    /* Inicio = indica que as threds só vão iniciar
    2 segundos depois da hora obtida do sistema,
    e terminam 2 segundos depois de terem iniciado */
    relogio.Inicio = hora_sistema_ms() + 2000;
    relogio.Fim = relogio.Inicio + 4000;

    pthread_create(&thread_id[0], NULL, &func1, NULL);
    pthread_create(&thread_id[1], NULL, &func2, NULL);
    pthread_create(&thread_id[2], NULL, &func3, NULL);

    for(i = 0; i < NUM_THREADS; ++i){
        pthread_join(thread_id[i], NULL);
    }

    printf("\n===========================================================\n");
	printf("===================== Fim do Programa! ====================\n");
	printf("===========================================================\n");

    return 0;
}

void *func1(void *arg){
    priorities(99);

    int i,j;
    double seconds;
    long int tempo_exe_1,tempo_comp_1;
    tempo_exe_1 = relogio.Inicio;

    for(i=0,j=0; tempo_exe_1 < relogio.Fim;++i){
        sleep_thr(tempo_exe_1);

        f1(2, 5);
  
        tempo_comp_1 =  hora_sistema_ms() - tempo_exe_1;

        seconds=(double)tempo_comp_1/(1e3); /* Conversão para segundos */
        printf("Tarefa 1: %f s\n",seconds);

        if(tempo_comp_1 < P1_Activacao) ++j;

        tempo_exe_1 += P1_Activacao; 
    }
    sleep_thr(tempo_exe_1+100); /* Espera que todas as threads terminem */
    printf("Percentagem de sucesso da Tarefa 1: %d%%\n",(100)*j/i);
    pthread_exit(NULL);
}

void *func2(void *arg){
    priorities(98);

    int i,j;
    double seconds;
    long int tempo_exe_2,tempo_comp_2;
    tempo_exe_2 = relogio.Inicio;

    for(i = 0,j = 0; tempo_exe_2 < relogio.Fim; ++i){
        sleep_thr(tempo_exe_2);

        f2(2, 5);

        tempo_comp_2 =  hora_sistema_ms() - tempo_exe_2;

        seconds = (double)tempo_comp_2/(1e3); /* Conversão para segundos */
        printf("Tarefa 2: %f s\n",seconds);

        if(tempo_comp_2 < P2_Activacao) ++j;

        tempo_exe_2 += P2_Activacao; 
    }
    sleep_thr(tempo_exe_2+100); /* Espera que todas as threads terminem */
    printf("Percentagem de sucesso da Tarefa 2: %d%%\n",(100)*j/i);
    pthread_exit(NULL);
}

void *func3(void *arg){
    priorities(97);
    
    int i,j;
    double seconds;
    long int tempo_exe_3,tempo_comp_3;
    tempo_exe_3 = relogio.Inicio;

    for(i = 0,j=0; tempo_exe_3 < relogio.Fim ; ++i){
    	sleep_thr(tempo_exe_3);
        
        f3(2, 5);
        
        tempo_comp_3 =  hora_sistema_ms() - tempo_exe_3;
        
        seconds = (double)tempo_comp_3/(1e3); /* Conversão para segundos */
        printf("Tarefa 3: %f s\n",seconds);

        if(tempo_comp_3 < P3_Activacao) ++j;

        /* É Calculado o próximo período de activação */
        tempo_exe_3 += P3_Activacao;
    }
    sleep_thr(tempo_exe_3+100); /* Espera que todas as threads terminem */
    printf("Percentagem de sucesso da Tarefa 3: %d%%\n",(100)*j/i);
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

/* Funão que recebe a hora do sistema com clock_gettime e */
/* devolve o resultado em milisegundos */
long int hora_sistema_ms(){
	long int ms,precisao_s;

	struct timespec tempo_actual;
	clock_gettime(CLOCK_MONOTONIC,&tempo_actual);

	ms = (tempo_actual.tv_nsec/1e6); /* Converte para milisegundos */

	/* É adicionado os milisegundos ao segundos para ser mais preciso
	Exemplo: 1*1e3=1000, então o tempo será 1+milisegundos que ficam nas
	3 ultimas casas */
	precisao_s=(tempo_actual.tv_sec*1e3)+ms; 
	
	return precisao_s;
}

/* Funão que recebe que as threads vão adormecer e */
/* e com o clock_nanosleep()  */
void sleep_thr(long int times){
	struct timespec t;
	t.tv_sec=times/(1e3);		/* Converte para segundos */

	/* Converte para nanosegundos
	Nesta linha, toma-se as 3 ultimas casas, através do resto da divisão,
	que vêm dos milisegundos calculados na linha 169, e depois é convertido para
	nanosegundos */
	t.tv_nsec=(times%1000)*(1e6);

	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
}
