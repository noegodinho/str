
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

/* Definição dos períodos de activação de cada tarefa em ms */
#define P1_Activacao 100
#define P2_Activacao 200
#define P3_Activacao 300

/* Definição de variaveis */
int enter=0;

/* Estrutura com o tempo de execução das tarefas */
struct tempo_execucao{
	long int Inicio,Fim;
}relogio;

/* Estrutura usada pelas threads para armazenar e usar informações importantes */
struct Dados_thread{
	/* Variaveis usadas para calcular a percentagem de sucesso de execuções
	das threads */
	int num_real_execucao;
	int num_execucao;
	int Percentagem;

	/* Variaveis para o tempo de execução das threads como também o tempo
	de computação */
	long int tempo_execucao;
	long int tempo_comp;
}Dados[3];

/* PL2, G5 */

void *func1(void *arg);
void *func2(void *arg);
void *func3(void *arg);

void priorities(int);
void sleep_thr(long int);
void outra_func();
long int hora_sistema_ms();

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
    relogio.Fim = relogio.Inicio + 2000;

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

    Dados[0].num_real_execucao=0;
    Dados[0].num_execucao=0;
    Dados[0].tempo_execucao = relogio.Inicio;

    for(; Dados[0].tempo_execucao < relogio.Fim ;){

        sleep_thr(Dados[0].tempo_execucao);

        f1(2, 5);
  
        Dados[0].tempo_comp =  hora_sistema_ms() - Dados[0].tempo_execucao;
        printf("Tarefa 1: %ld \tms\n",Dados[0].tempo_comp);
        Dados[0].num_real_execucao++;

        /* É incrementado o j para calcular a percentagem de sucesso tarefa */
        if(Dados[0].tempo_comp < P1_Activacao) Dados[0].num_execucao++;

        /* Calculo do proximo período de activação da tarefa */
        Dados[0].tempo_execucao += P1_Activacao;
    }

    sleep_thr(Dados[0].tempo_execucao+100); /* Espera que todas as threads terminem */
    outra_func();

    Dados[0].Percentagem = 100*Dados[0].num_execucao/Dados[0].num_real_execucao;
    printf("Percentagem de sucesso da Tarefa 1: %d%%\n",Dados[0].Percentagem);
    pthread_exit(NULL);
}

void *func2(void *arg){
    priorities(98);

    Dados[1].num_real_execucao=0;
    Dados[1].num_execucao=0;
    Dados[1].tempo_execucao = relogio.Inicio;

    for(; Dados[1].tempo_execucao < relogio.Fim ;){

        sleep_thr(Dados[1].tempo_execucao);

        f2(2, 5);

        Dados[1].tempo_comp =  hora_sistema_ms() - Dados[1].tempo_execucao;
        printf("Tarefa 2: %ld \tms\n",Dados[1].tempo_comp);
        Dados[1].num_real_execucao++;

        /* É incrementado o j para calcular a percentagem de sucesso tarefa */
        if(Dados[1].tempo_comp < P2_Activacao) Dados[1].num_execucao++;

        /* Calculo do proximo período de activação da tarefa */
        Dados[1].tempo_execucao += P2_Activacao;
    }

    sleep_thr(Dados[1].tempo_execucao+100); /* Espera que todas as threads terminem */
    outra_func();

    Dados[1].Percentagem = 100*Dados[1].num_execucao/Dados[1].num_real_execucao;
    printf("Percentagem de sucesso da Tarefa 2: %d%%\n",Dados[1].Percentagem);
    pthread_exit(NULL);
}

void *func3(void *arg){
    priorities(97);
    
    Dados[2].num_real_execucao=0;
    Dados[2].num_execucao=0;
    Dados[2].tempo_execucao = relogio.Inicio;

    for(; Dados[2].tempo_execucao < relogio.Fim ;){
    	
    	sleep_thr(Dados[2].tempo_execucao);
        
        f3(2, 5);
        
        Dados[2].tempo_comp =  hora_sistema_ms() - Dados[2].tempo_execucao;  
        printf("Tarefa 3: %ld \tms\n",Dados[2].tempo_comp);
        Dados[2].num_real_execucao++;

        /* É incrementado o j para calcular a percentagem de sucesso tarefa */
        if(Dados[2].tempo_comp < P3_Activacao) Dados[2].num_execucao++;

        /* Calculo do proximo período de activação da tarefa */
        Dados[2].tempo_execucao += P3_Activacao;
    }

    sleep_thr(Dados[2].tempo_execucao+100); /* Espera que todas as threads terminem */
    outra_func();

    Dados[2].Percentagem = 100*Dados[2].num_execucao/Dados[2].num_real_execucao;
    printf("Percentagem de sucesso da Tarefa 3: %d%%\n",Dados[2].Percentagem);
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
	long int ms,precisao_ms;

	struct timespec tempo_actual;
	clock_gettime(CLOCK_MONOTONIC,&tempo_actual);

	/* Converte para milisegundos
	1ns=1*10^-9, para milisegundos fica (1*10^-9)*(1*10^-6)=1*10^-3 */
	ms = tempo_actual.tv_nsec/1e6;

	/* ## Conversão de segundos para milisegundos, 1 seg = 1*10^3 ms
	É adicionado os milisegundos ao segundos para ser mais preciso
	Exemplo: 1*1e3=1000, então o tempo será 1+milisegundos que ficam nas
	3 ultimas casas */
	precisao_ms=(tempo_actual.tv_sec*1e3)+ms; 
	
	return precisao_ms;
}

/* Funão que recebe que as threads vão adormecer e */
/* e com o clock_nanosleep()  */
void sleep_thr(long int times){
	struct timespec t;
	t.tv_sec=times/(1e3);		/* Converte para segundos */

	/* Converte para nanosegundos
	Nesta linha, toma-se as 3 ultimas casas, através do resto da divisão,
	que vêm dos milisegundos calculados na linha ##, e depois é convertido para
	nanosegundos. Como para converter para ms dividi ns/1e6, então para converter
	novamente para nanosegundos é só multiplicar por 1e6 */
	t.tv_nsec=(times%1000)*(1e6);

	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
}

/* Funcçao usada para fazer a mudança de linha e imprimir
uma mensagem quando é feita a mudança de prioridade */
void outra_func(){
    if(enter==0){
        printf("\n");
        enter=1;
    }
}
