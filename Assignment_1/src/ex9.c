/****************************************************************
 * Autor: José Manuel C. Noronha
 * Autor: Noé Godinho
 * Turma: PL2
 * Grupo: 5
 * Ano Lectivo: 2016 - 2017
 ***************************************************************/

/* Includes, Definições e Notas */
#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <sys/mman.h>
#include <stdbool.h>
#include "../lib/func.h"
#include "../lib/ex8.h"

#define NUM_THREADS 3
#define INDICE_THR_IMPRIMIR 1000

/* Definição dos períodos de activação de cada tarefa em ms */
#define P1_Activacao 100
#define P2_Activacao 200
#define P3_Activacao 300

/* Definição de variaveis */
int id_thr_imprimir[INDICE_THR_IMPRIMIR];
long int tempo_comp_thread_imprimir[INDICE_THR_IMPRIMIR];
int indice_a_ser_incrementado = -1;
int confirma_passagem_thrs = -1;
struct timespec Inicio;
struct timespec tempo_inv;

/* ms = milisegundos
 * Nos comentários tarefas = threads */

/* Estrutura usada pelas threads para armazenar e usar informações importantes */
struct Dados_thread{
	/* Variável usada para fazer P1_Activacao para o timespec da thread */
	struct timespec Periodo;

	/* Variaveis usadas para calcular a percentagem de sucesso de execuções
	das threads */
	int num_real_execucao;
	int num_execucao;
	int Percentagem[2];

	/* Variaveis para o tempo de computação threads */
	long int tempo_comp;

	/* Usada para fazermos a verificação se a mudança de prioridade
	foi feita  ou não */
	bool mudanca_prioridade;
}Dados[3];

/* Definiçoes das Funções */
void *func1(void *arg);
void *func2(void *arg);
void *func3(void *arg);
void priorities();
long int hora_sistema_ms();
void imprimir();
void hora_actual_introducao_thr();

int main(){
	printf("===========================================================\n");
	printf("================== Iniciando o Programa! ==================\n");
	printf("=========== Sistemas de Tempo Real, PL2, Grupo5 ===========\n");
	printf("===========================================================\n\n");

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

	/* Definição dos periodos
	 * 1ms = 1*10^-3, assim convertendo para nanosegundos temos
	 * que multiplicar por 1*10^-6 para o resultado ser 1*10^-9,
	 * logo, em formato long temos mili=1*10^3, assim para ser
	 * convertido em nano temos que: mili * 1*10^6 = 1*10^9 em
	 * formato long */
	Dados[0].Periodo.tv_sec = 0;
	Dados[0].Periodo.tv_nsec = P1_Activacao * 1e6;

	Dados[1].Periodo.tv_sec = 0;
	Dados[1].Periodo.tv_nsec = P2_Activacao * 1e6;

	Dados[2].Periodo.tv_sec = 0;
	Dados[2].Periodo.tv_nsec = P3_Activacao * 1e6;

	/* Funcção para obter a hora actual para a thread */
	hora_actual_introducao_thr();

	/* O tempo para inverter as prioridades é feito 2 segundos depois de
	 * as threads iniciarem */
	tempo_inv.tv_sec = Inicio.tv_sec + 1;
	tempo_inv.tv_nsec = Inicio.tv_nsec;

	/* Introdução dos dados referente a thred 1 */
	new_rt_task_make_periodic(0, 99, Inicio, Dados[0].Periodo, 2);
	/* Introdução dos dados referente a thred 2 */
	new_rt_task_make_periodic(1, 98, Inicio, Dados[1].Periodo, 2);
	/* Introdução dos dados referente a thred 3 */
	new_rt_task_make_periodic(2, 97, Inicio, Dados[2].Periodo, 2);

	/* cria as threads */
	pthread_create(&thread_id[0], NULL, &func1, NULL);
	pthread_create(&thread_id[1], NULL, &func2, NULL);
	pthread_create(&thread_id[2], NULL, &func3, NULL);

	/* espera a que as threads terminem */
	for(i = 0; i < NUM_THREADS; ++i){
		pthread_join(thread_id[i], NULL);
	}

	/* Imprimi as informações armazenadas durante a execução */
	imprimir();

	printf("\n===========================================================\n");
	printf("===================== Fim do Programa! ====================\n");
	printf("===========================================================\n");

	return 0;
}

/* Função para thread 1 */
void *func1(void *arg){
	int indice = 0;
	/* define a prioridade da thread */
	priorities();

	/* Inicialização das variaveis */
  Dados[indice].num_real_execucao=0;
  Dados[indice].num_execucao=0;
	/* Variável usada para confirmar que a mudança de prioridade
	 * não foi feita */
  Dados[indice].mudanca_prioridade=false;

	/* Ciclo que executa a thread até o tempo definido para a mesma terminar */
  for(; hora_sistema_ms() < (thread_info[indice].end.tv_sec*BILLION + thread_info[indice].end.tv_nsec) ;){

		/* Tempo usado para calcular o tempo de computação */
		Dados[indice].tempo_comp = thread_info[indice].start.tv_sec*BILLION + thread_info[indice].start.tv_nsec;

		/* A thread adormece até o tempo definido */
	    new_rt_task_wait_period(0);

	    f1(2, 5);

		//Dados[indice].tempo_comp -= thread_info[indice].period.tv_nsec;
		//printf("I %ld - C %ld - H %ld\n",Inicio.tv_sec*BILLION+Inicio.tv_nsec, Dados[indice].tempo_comp,hora_sistema_ms());

		/* Calculo do tempo de computação da thread, através da diferença
		 * entre a hora actual do sistema com o tempo quando a thread
		 * iniciou a sua execução */
		Dados[indice].tempo_comp = hora_sistema_ms() - Dados[indice].tempo_comp;

		/* Valores usados para fazer a impressão das informações das threads
		 * onde uma delas identifica a thread e a outra recebe o valor do tempo
		 * de computação */
		indice_a_ser_incrementado++;
		id_thr_imprimir[indice_a_ser_incrementado] = 1;
		tempo_comp_thread_imprimir[indice_a_ser_incrementado] = Dados[indice].tempo_comp;

		/* Variável usada para calcular a percentagem do numero de vezes que a
		 * thread cumpriu, ou nao, a meta. Ou seja, para calcular a percentagem
		 * de sucesso tarefa */
   		Dados[indice].num_real_execucao++;

		/* É incrementado o num_execucao caso a thread não consiga atingir a meta.
		 * Esse valor é usado para calcular a percentagem de sucesso tarefa */
	    if(Dados[indice].tempo_comp < thread_info[indice].period.tv_nsec) Dados[indice].num_execucao++;

			/* Condição que verifica se o tempo de execução da thread chegou ao Fim
			 * e como também, verifica se a mudança de prioridade
			 * já foi feita ou não */
	    if(hora_sistema_ms() > (tempo_inv.tv_sec*BILLION + tempo_inv.tv_nsec) && !Dados[indice].mudanca_prioridade){
			/* O calculo da precentagem, antes da mudança é feito aqui */
	    	Dados[indice].Percentagem[0] = 100*Dados[indice].num_execucao/Dados[indice].num_real_execucao;

				/* Variável usada para confirmar que a mudança de prioridade
				 * já foi feita, logo não é necessário entrar de novo no if */
	    	Dados[indice].mudanca_prioridade = true;

				/* Reinicia as variaveis usadas para o calculo da percentagem */
	    	Dados[indice].num_execucao=Dados[indice].num_real_execucao=0;

			/* Condição usada para verificar se já foi armazendo
		 	* o valor -5 na tabela por outra thread, e caso não
		 	* se confirme, então armazena o valor na tabela */
			if(confirma_passagem_thrs == -1){
			/* Valor usado para garantir que deve imprimir que foi feita
			 * feita a mudança de variável */
				indice_a_ser_incrementado++;
				id_thr_imprimir[indice_a_ser_incrementado] = -5;

				/* Funcção para obter a hora actual para a thread */
				hora_actual_introducao_thr();

				/* Já foi armazenado o valor, e por isso, mais nenhuma
				 * thread o pode fazer */
				confirma_passagem_thrs = 0;
			}

			/* Introdução dos dados referente a thred 1 */
			new_rt_task_make_periodic(indice, 97, Inicio, Dados[indice].Periodo, 1);

			/* define a prioridade da thread */
			priorities(0);
		}
  	}

	/* O calculo da precentagem, depois da mudança é feito aqui */
	Dados[indice].Percentagem[1] = 100*Dados[indice].num_execucao/Dados[indice].num_real_execucao;
	/* faz exit da thread */
	pthread_exit(NULL);
}

void *func2(void *arg){
	int indice = 1;
	/* define a prioridade da thread */
	priorities();

	/* Inicialização das variaveis */
	Dados[indice].num_real_execucao=0;
	Dados[indice].num_execucao=0;
	/* Variável usada para confirmar que a mudança de prioridade
	 * não foi feita */
	Dados[indice].mudanca_prioridade=false;

	/* Ciclo que executa a thread até o tempo definido para a mesma terminar */
	for(; hora_sistema_ms() < (thread_info[indice].end.tv_sec*BILLION + thread_info[indice].end.tv_nsec) ;){

		/* Tempo usado para calcular o tempo de computação */
		Dados[indice].tempo_comp = thread_info[indice].start.tv_sec*BILLION + thread_info[indice].start.tv_nsec;

		/* A thread adormece até o tempo definido */
		new_rt_task_wait_period(1);

		f1(2, 5);

		/* Calculo do tempo de computação da thread, através da diferença
		 * entre a hora actual do sistema com o tempo quando a thread
		 * iniciou a sua execução */

		Dados[indice].tempo_comp = hora_sistema_ms() - Dados[indice].tempo_comp;

		/* Valores usados para fazer a impressão das informações das threads
		 * onde uma delas identifica a thread e a outra recebe o valor do tempo
		 * de computação */
		indice_a_ser_incrementado++;
		id_thr_imprimir[indice_a_ser_incrementado] = 2;
		tempo_comp_thread_imprimir[indice_a_ser_incrementado] = Dados[indice].tempo_comp;

		/* Variável usada para calcular a percentagem do numero de vezes que a
		 * thread cumpriu, ou nao, a meta. Ou seja, para calcular a percentagem
		 * de sucesso tarefa */
		Dados[indice].num_real_execucao++;

		/* É incrementado o num_execucao caso a thread não consiga atingir a meta.
		 * Esse valor é usado para calcular a percentagem de sucesso tarefa */
		if(Dados[indice].tempo_comp < thread_info[indice].period.tv_nsec) Dados[indice].num_execucao++;

		/* Condição que verifica se o tempo de execução da thread chegou ao Fim
		 * e como também, verifica se a mudança de prioridade
		 * já foi feita ou não */
		if(hora_sistema_ms() > (tempo_inv.tv_sec*BILLION + tempo_inv.tv_nsec) && !Dados[indice].mudanca_prioridade){
			/* O calculo da precentagem, antes da mudança é feito aqui */
			Dados[indice].Percentagem[0] = 100*Dados[indice].num_execucao/Dados[indice].num_real_execucao;

			/* Variável usada para confirmar que a mudança de prioridade
			 * já foi feita, logo não é necessário entrar de novo no if */
			Dados[indice].mudanca_prioridade = true;

			/* Reinicia as variaveis usadas para o calculo da percentagem */
			Dados[indice].num_execucao=Dados[indice].num_real_execucao=0;

			/* Condição usada para verificar se já foi armazendo
			 * o valor -5 na tabela por outra thread, e caso não
			 * se confirme, então armazena o valor na tabela */
			if(confirma_passagem_thrs == -1){
				/* Valor usado para garantir que deve imprimir que foi feita
				 * feita a mudança de variável */
				indice_a_ser_incrementado++;
				id_thr_imprimir[indice_a_ser_incrementado] = -5;

				/* Funcção para obter a hora actual para a thread */
				hora_actual_introducao_thr();

				/* Já foi armazenado o valor, e por isso, mais nenhuma
				 * thread o pode fazer */
				confirma_passagem_thrs = 0;
			}

			/* Introdução dos dados referente a thred 1 */
			new_rt_task_make_periodic(indice, 98, Inicio, Dados[indice].Periodo, 1);

			/* define a prioridade da thread */
			priorities(1);
		}
	}

	/* O calculo da precentagem, depois da mudança é feito aqui */
	Dados[indice].Percentagem[1] = 100*Dados[indice].num_execucao/Dados[indice].num_real_execucao;
	/* faz exit da thread */
  pthread_exit(NULL);
}

void *func3(void *arg){
	int indice = 2;
	/* define a prioridade da thread */
	priorities();

	/* Inicialização das variaveis */
	Dados[indice].num_real_execucao=0;
	Dados[indice].num_execucao=0;
	/* Variável usada para confirmar que a mudança de prioridade
	 * não foi feita */
	Dados[indice].mudanca_prioridade=false;

	/* Ciclo que executa a thread até o tempo definido para a mesma terminar */
	for(; hora_sistema_ms() < (thread_info[indice].end.tv_sec*BILLION + thread_info[indice].end.tv_nsec) ;){

		/* Tempo usado para calcular o tempo de computação */
		Dados[indice].tempo_comp = thread_info[indice].start.tv_sec*BILLION + thread_info[indice].start.tv_nsec;

		/* A thread adormece até o tempo definido */
		new_rt_task_wait_period(2);

		f1(2, 5);

		/* Calculo do tempo de computação da thread, através da diferença
		 * entre a hora actual do sistema com o tempo quando a thread
		 * iniciou a sua execução */

		Dados[indice].tempo_comp = hora_sistema_ms() - Dados[indice].tempo_comp;

		/* Valores usados para fazer a impressão das informações das threads
		 * onde uma delas identifica a thread e a outra recebe o valor do tempo
		 * de computação */
		indice_a_ser_incrementado++;
		id_thr_imprimir[indice_a_ser_incrementado] = 3;
		tempo_comp_thread_imprimir[indice_a_ser_incrementado] = Dados[indice].tempo_comp;

		/* Variável usada para calcular a percentagem do numero de vezes que a
		 * thread cumpriu, ou nao, a meta. Ou seja, para calcular a percentagem
		 * de sucesso tarefa */
		Dados[indice].num_real_execucao++;

		/* É incrementado o num_execucao caso a thread não consiga atingir a meta.
		 * Esse valor é usado para calcular a percentagem de sucesso tarefa */
		if(Dados[indice].tempo_comp < thread_info[indice].period.tv_nsec) Dados[indice].num_execucao++;

		/* Condição que verifica se o tempo de execução da thread chegou ao Fim
		 * e como também, verifica se a mudança de prioridade
		 * já foi feita ou não */
		if(hora_sistema_ms() > (tempo_inv.tv_sec*BILLION + tempo_inv.tv_nsec) && !Dados[indice].mudanca_prioridade){
			/* O calculo da precentagem, antes da mudança é feito aqui */
			Dados[indice].Percentagem[0] = 100*Dados[indice].num_execucao/Dados[indice].num_real_execucao;

			/* Variável usada para confirmar que a mudança de prioridade
			 * já foi feita, logo não é necessário entrar de novo no if */
			Dados[indice].mudanca_prioridade = true;

			/* Reinicia as variaveis usadas para o calculo da percentagem */
			Dados[indice].num_execucao=Dados[indice].num_real_execucao=0;

			/* Condição usada para verificar se já foi armazendo
			 * o valor -5 na tabela por outra thread, e caso não
			 * se confirme, então armazena o valor na tabela */
			if(confirma_passagem_thrs == -1){
				/* Valor usado para garantir que deve imprimir que foi feita
				 * feita a mudança de variável */
				indice_a_ser_incrementado++;
				id_thr_imprimir[indice_a_ser_incrementado] = -5;

				/* Funcção para obter a hora actual para a thread */
				hora_actual_introducao_thr();

				/* Já foi armazenado o valor, e por isso, mais nenhuma
				 * thread o pode fazer */
				confirma_passagem_thrs = 0;
			}

			/* Introdução dos dados referente a thred 1 */
			new_rt_task_make_periodic(indice, 99, Inicio, Dados[indice].Periodo, 1);

			/* define a prioridade da thread */
			priorities(2);
		}
	}

	/* O calculo da precentagem, depois da mudança é feito aqui */
	Dados[indice].Percentagem[1] = 100*Dados[indice].num_execucao/Dados[indice].num_real_execucao;
	/* faz exit da thread */
	pthread_exit(NULL);
}

/* Função que define o valor das prioridades das threads */
void priorities(int i){
	pthread_t id = pthread_self();
	struct sched_param param;
	
	/* define o valor da prioridade recebido */
	param.sched_priority = thread_info[i].priority;

		/* aplica um escalonamento RMPO */
	if(pthread_setschedparam(id, SCHED_FIFO, &param) != 0){
		perror("Error from pthread_setschedparam");
	}
}

/* Funão que recebe a hora do sistema com clock_gettime e
 * devolve esse mesmo tempo obtido */
long int hora_sistema_ms(){
	struct timespec tempo_actual;
	clock_gettime(CLOCK_MONOTONIC,&tempo_actual);

	return (tempo_actual.tv_sec*BILLION + tempo_actual.tv_nsec);
}

/* Funcçao usada para imprimir as informações que foram armazenadas
 * durante a execução das threads */
void imprimir(){
	for(int i=0 ; i <= indice_a_ser_incrementado ; ++i ){
		if(id_thr_imprimir[i] == -5){
			printf("\nPercentagem de sucesso da Tarefa 1: %d%%\n",Dados[0].Percentagem[0]);
			printf("Percentagem de sucesso da Tarefa 2: %d%%\n",Dados[1].Percentagem[0]);
			printf("Percentagem de sucesso da Tarefa 3: %d%%\n",Dados[2].Percentagem[0]);

			printf("\n\n============= Mudanca de Prioridade =============\n\n");
		}
		else{
			printf("Tarefa %d: %ld \tms\n",id_thr_imprimir[i],tempo_comp_thread_imprimir[i]);
		}
	}

	printf("\nPercentagem de sucesso da Tarefa 1: %d%%\n",Dados[0].Percentagem[1]);
	printf("Percentagem de sucesso da Tarefa 2: %d%%\n",Dados[1].Percentagem[1]);
	printf("Percentagem de sucesso da Tarefa 3: %d%%\n",Dados[2].Percentagem[1]);
}

/* Funcção para obter a hora actual para introduzir nas
 * funcções rt_task */
void hora_actual_introducao_thr(){
	/* Essa condição evita alterações multiplas das threads */
	if(confirma_passagem_thrs == -1){
		/* Definição do tempo que iniciam as threds, ou seja, as threds
		 * devem iniciar 2 segundos depois do tempo obtido através do sistema */
		clock_gettime(CLOCK_MONOTONIC,&Inicio);
		Inicio.tv_sec += 2;
	}
}
