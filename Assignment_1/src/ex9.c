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
#include "../lib/func.h"
#include "../lib/ex8.h"

#define INDICE_IMPRIMIR 1000	/* Tamanho maximo da tabela que armazena info */

/* Definição dos períodos de activação de cada tarefa em ms */
#define P1_Activacao 100
#define P2_Activacao 200
#define P3_Activacao 300

/* Estrutura onde é armazenada as informações para imprimir */
struct Armazena_Dados_Imprimir{
	int id_thr_imprimir[INDICE_IMPRIMIR];
	long int t_comp_imprimir[INDICE_IMPRIMIR];
	int indice_tab;
	int confirma_mud_pri;
}Imprimir;

/* ms = milisegundos
 * Nos comentários tarefas = threads */

/* Estrutura com o tempo de execução das tarefas */
struct Tempo_Execucao{
	struct timespec inicio;
	struct timespec t_inv;
}Relogio;

/* Estrutura usada pelas threads */
struct Dados_Thread{
	/* Variaveis para o periodo das threads */
	struct timespec periodo;

	/* Variaveis usadas para calcular a percentagem de sucesso de execuções
	 * das threads */
	int num_real_execucao;
	int num_execucao;
	int percentagem[2];

	/* Variável usada para o tempo de calculo */
	long int tempo_comp;

	/* Usada para fazermos a verificação se a mudança de prioridade
	foi feita  ou não */
	int mudanca_prioridade;
}Dados_thr[MAX_THREADS];

/* Definiçoes das Funções */
void *func1(void *arg);
void *func2(void *arg);
void *func3(void *arg);
void priorities(int);
void sleep_thr(int);
long int hora_sistema();
void imprimir();

int main(int argc, char** argv){
	printf("===========================================================\n");
	printf("================== Iniciando o Programa! ==================\n");
	printf("=========== Sistemas de Tempo Real, PL2, Grupo5 ===========\n");
	printf("===========================================================\n\n");

  cpu_set_t set;
  pthread_t thread_id[MAX_THREADS];
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

	/* Inicialização do Indice da tabela que é usado para armazenar info */
	Imprimir.indice_tab = -1;

	/* Variavel usada duante a impressão, sabermos quando houve mudança
	 * de prioridade */
	Imprimir.confirma_mud_pri = -1;

  /* Inicio = indica que as threads só vão iniciar 2 segundos depois
	 * da hora obtida do sistema(com clock_gettime) */
	clock_gettime(CLOCK_MONOTONIC,&Relogio.inicio);
	Relogio.inicio.tv_sec += 2;

	/* O tempo para inverter as prioridades é feito 2 segundos depois de
	 * as threads iniciarem */
	Relogio.t_inv.tv_sec = Relogio.inicio.tv_sec + 2;
	Relogio.t_inv.tv_nsec = Relogio.inicio.tv_nsec;

	/* cria as threads */
  pthread_create(&thread_id[0], NULL, &func1, NULL);
  pthread_create(&thread_id[1], NULL, &func2, NULL);
  pthread_create(&thread_id[2], NULL, &func3, NULL);

	/* espera a que as threads terminem */
  for(i = 0; i < MAX_THREADS; ++i){
		pthread_join(thread_id[i], NULL);
  }

	/* Imprimi as informações armazenadas durante a execução */
	imprimir();

	printf("\n===========================================================\n");
	printf("===================== Fim do Programa! ====================\n");
	printf("===========================================================\n");

  return 0;
}

/* Função que define o valor das prioridades das threads */
void priorities(int i){
	/* vai buscar o id da thread */
	pthread_t id = pthread_self();

	/* define o valor da prioridade recebido */
	struct sched_param param;
  param.sched_priority = thread_info[i].priority;

	/* aplica um escalonamento RMPO */
  if(pthread_setschedparam(id, SCHED_FIFO, &param) != 0){
      perror("Error from pthread_setschedparam");
  }
}

/* Função usada pelas threads, com o objectivo de receber a hora
 * do sistema com clock_gettime e devolver esse mesmo tempo */
long int hora_sistema(){
	struct timespec tempo_actual;
	clock_gettime(CLOCK_MONOTONIC,&tempo_actual);
	return (tempo_actual.tv_sec*BILLION + tempo_actual.tv_nsec);
}

/* Função para thread 1 */
void *func1(void *arg){
	/* Variavel usada para indicar o indice da tabela de threads */
	int i = 0;

	/* Inicialização das variaveis */
	Dados_thr[i].num_real_execucao = 0;
	Dados_thr[i].num_execucao = 0;
	Dados_thr[i].mudanca_prioridade = 0;

	/* Definição do periodo */
	Dados_thr[i].periodo.tv_sec = 0;
	Dados_thr[i].periodo.tv_nsec = P1_Activacao * 1e6;

	/* Introdução dos dados referente a thred 1 */
	new_rt_task_make_periodic(i, 99, Relogio.inicio, Dados_thr[i].periodo, 3);

	/* Variavel usada para armazenar o tempo de activação/execução
	 * em formato long int */
	long int tempo_thr;
	tempo_thr = thread_info[i].start.tv_sec*BILLION + thread_info[i].start.tv_nsec;

	/* define a prioridade da thread */
  priorities(i);

	/* Ciclo que executa a thread até o tempo definido para a mesma terminar */
  for(;tempo_thr < (thread_info[i].end.tv_sec*BILLION+thread_info[i].end.tv_nsec);){

		/* A thread adormece até o tempo definido */
		new_rt_task_wait_period(i);

    f1(2, 5);

		/* Calculo do tempo de computação da thread, através da diferença
		 * entre a hora actual do sistema com o tempo quando a thread
		 * iniciou a sua execução */
    Dados_thr[i].tempo_comp =  hora_sistema() - tempo_thr;

		/* Valores usados para fazer a impressão das informações das threads
		 * onde uma delas identifica a thread e a outra recebe o valor do tempo
		 * de computação */
		Imprimir.indice_tab++;
		Imprimir.id_thr_imprimir[Imprimir.indice_tab] = 1;
		Imprimir.t_comp_imprimir[Imprimir.indice_tab] = Dados_thr[i].tempo_comp/CLOCKS_PER_SEC;

		/* Variável usada para calcular a percentagem do numero de vezes que a
		 * thread cumpriu, ou nao, a meta. Ou seja, para calcular a percentagem
		 * de sucesso tarefa */
		Dados_thr[i].num_real_execucao++;

    /* É incrementado o num_execucao caso a thread não consiga atingir a meta.
		 * Esse valor é usado para calcular a percentagem de sucesso tarefa */
    if(Dados_thr[i].tempo_comp < thread_info[i].period.tv_nsec){
			Dados_thr[i].num_execucao++;
		}

		tempo_thr = thread_info[i].start.tv_sec*BILLION + thread_info[i].start.tv_nsec;

		/* Condição que verifica se o tempo de execução da thread chegou ao Fim
		 * e como também, verifica se a mudança de prioridade
		 * já foi feita ou não */
		if((tempo_thr > (Relogio.t_inv.tv_sec*BILLION+Relogio.t_inv.tv_nsec)) && Dados_thr[i].mudanca_prioridade == 0){

			/* Espera 1 seguando, para que as outras threads terminem */
			struct timespec espera_thr;
			espera_thr.tv_sec = 1;
			espera_thr.tv_nsec = 0;
			clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &espera_thr, NULL);

			/* O calculo da precentagem, antes da mudança é feito aqui */
      Dados_thr[i].percentagem[0] = 100*Dados_thr[i].num_execucao/Dados_thr[i].num_real_execucao;

			/* Re-definição das variaveis */
			Dados_thr[i].num_real_execucao = 0;
			Dados_thr[i].num_execucao = 0;

			/* Variável usada para confirmar que a mudança de prioridade
			 * já foi feita, logo não é necessário entrar de novo no if */
			Dados_thr[i].mudanca_prioridade = 1;

			/* Condição usada para verificar se já foi armazendo
			 * o valor -5 na tabela por outra thread, e caso não
			 * se confirme, então armazena o valor na tabela */
			if(Imprimir.confirma_mud_pri == -1){
				/* Valor usado para garantir que deve imprimir que foi feita
				 * feita a mudança de variável */
				Imprimir.indice_tab++;
				Imprimir.id_thr_imprimir[Imprimir.indice_tab] = -5;

				/* Já foi armazenado o valor, e por isso, mais nenhuma
				 * thread o pode fazer */
				Imprimir.confirma_mud_pri = 0;

				/* Inicio = indica que as threads só vão iniciar 2 segundos depois
				 * da hora obtida do sistema(com clock_gettime)  */
				clock_gettime(CLOCK_MONOTONIC,&Relogio.inicio);
				Relogio.inicio.tv_sec += 2;
			}

			/* Introdução dos dados referente a thred 1 */
			new_rt_task_make_periodic(i, 97, Relogio.inicio, Dados_thr[i].periodo, 2);

			tempo_thr = thread_info[i].start.tv_sec*BILLION + thread_info[i].start.tv_nsec;

			/* define a prioridade da thread */
		  priorities(i);
		}
  }

	/* O calculo da precentagem é feito aqui */
  Dados_thr[i].percentagem[1] = 100*Dados_thr[i].num_execucao/Dados_thr[i].num_real_execucao;
	/* faz exit da thread */
  pthread_exit(NULL);
}

/* Função para thread 2 */
void *func2(void *arg){
	/* Variavel usada para indicar o indice da tabela de threads */
	int i = 1;

	/* Inicialização das variaveis */
	Dados_thr[i].num_real_execucao = 0;
	Dados_thr[i].num_execucao = 0;
	Dados_thr[i].mudanca_prioridade = 0;

	/* Definição do periodo */
	Dados_thr[i].periodo.tv_sec = 0;
	Dados_thr[i].periodo.tv_nsec = P2_Activacao * 1e6;

	/* Introdução dos dados referente a thred 1 */
	new_rt_task_make_periodic(i, 98, Relogio.inicio, Dados_thr[i].periodo, 3);

	/* Variavel usada para armazenar o tempo de activação/execução
	 * em formato long int */
	long int tempo_thr;
	tempo_thr = thread_info[i].start.tv_sec*BILLION + thread_info[i].start.tv_nsec;

	/* define a prioridade da thread */
  priorities(i);

	/* Ciclo que executa a thread até o tempo definido para a mesma terminar */
  for(;tempo_thr < (thread_info[i].end.tv_sec*BILLION+thread_info[i].end.tv_nsec);){

		/* A thread adormece até o tempo definido */
		new_rt_task_wait_period(i);

    f2(2, 5);

		/* Calculo do tempo de computação da thread, através da diferença
		 * entre a hora actual do sistema com o tempo quando a thread
		 * iniciou a sua execução */
    Dados_thr[i].tempo_comp =  hora_sistema() - tempo_thr;

		/* Valores usados para fazer a impressão das informações das threads
		 * onde uma delas identifica a thread e a outra recebe o valor do tempo
		 * de computação */
		Imprimir.indice_tab++;
		Imprimir.id_thr_imprimir[Imprimir.indice_tab] = 2;
		Imprimir.t_comp_imprimir[Imprimir.indice_tab] = Dados_thr[i].tempo_comp/CLOCKS_PER_SEC;

		/* Variável usada para calcular a percentagem do numero de vezes que a
		 * thread cumpriu, ou nao, a meta. Ou seja, para calcular a percentagem
		 * de sucesso tarefa */
		Dados_thr[i].num_real_execucao++;

    /* É incrementado o num_execucao caso a thread não consiga atingir a meta.
		 * Esse valor é usado para calcular a percentagem de sucesso tarefa */
    if(Dados_thr[i].tempo_comp < thread_info[i].period.tv_nsec){
			Dados_thr[i].num_execucao++;
		}

		tempo_thr = thread_info[i].start.tv_sec*BILLION + thread_info[i].start.tv_nsec;

		/* Condição que verifica se o tempo de execução da thread chegou ao Fim
		 * e como também, verifica se a mudança de prioridade
		 * já foi feita ou não */
		if((tempo_thr > (Relogio.t_inv.tv_sec*BILLION+Relogio.t_inv.tv_nsec)) && Dados_thr[i].mudanca_prioridade == 0){

			/* Espera 1 seguando, para que as outras threads terminem */
			struct timespec espera_thr;
			espera_thr.tv_sec = 1;
			espera_thr.tv_nsec = 0;
			clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &espera_thr, NULL);

			/* O calculo da precentagem, antes da mudança é feito aqui */
      Dados_thr[i].percentagem[0] = 100*Dados_thr[i].num_execucao/Dados_thr[i].num_real_execucao;

			/* Re-definição das variaveis */
			Dados_thr[i].num_real_execucao = 0;
			Dados_thr[i].num_execucao = 0;

			/* Variável usada para confirmar que a mudança de prioridade
			 * já foi feita, logo não é necessário entrar de novo no if */
			Dados_thr[i].mudanca_prioridade = 1;

			/* Condição usada para verificar se já foi armazendo
			 * o valor -5 na tabela por outra thread, e caso não
			 * se confirme, então armazena o valor na tabela */
			if(Imprimir.confirma_mud_pri == -1){
				/* Valor usado para garantir que deve imprimir que foi feita
				 * feita a mudança de variável */
				Imprimir.indice_tab++;
				Imprimir.id_thr_imprimir[Imprimir.indice_tab] = -5;

				/* Já foi armazenado o valor, e por isso, mais nenhuma
				 * thread o pode fazer */
				Imprimir.confirma_mud_pri = 0;

				/* Inicio = indica que as threads só vão iniciar 2 segundos depois
				 * da hora obtida do sistema(com clock_gettime)  */
				clock_gettime(CLOCK_MONOTONIC,&Relogio.inicio);
				Relogio.inicio.tv_sec += 2;
			}

			/* Introdução dos dados referente a thred 1 */
			new_rt_task_make_periodic(i, 98, Relogio.inicio, Dados_thr[i].periodo, 2);

			tempo_thr = thread_info[i].start.tv_sec*BILLION + thread_info[i].start.tv_nsec;

			/* define a prioridade da thread */
		  priorities(i);
		}
  }

	/* O calculo da precentagem é feito aqui */
  Dados_thr[i].percentagem[1] = 100*Dados_thr[i].num_execucao/Dados_thr[i].num_real_execucao;
  /* faz exit da thread */
  pthread_exit(NULL);
}

/* Função para thread 3 */
void *func3(void *arg){
	/* Variavel usada para indicar o indice da tabela de threads */
	int i = 2;

	/* Inicialização das variaveis */
	Dados_thr[i].num_real_execucao = 0;
	Dados_thr[i].num_execucao = 0;
	Dados_thr[i].mudanca_prioridade = 0;

	/* Definição do periodo */
	Dados_thr[i].periodo.tv_sec = 0;
	Dados_thr[i].periodo.tv_nsec = P3_Activacao * 1e6;

	/* Introdução dos dados referente a thred 1 */
	new_rt_task_make_periodic(i, 97, Relogio.inicio, Dados_thr[i].periodo, 3);

	/* Variavel usada para armazenar o tempo de activação/execução
	 * em formato long int */
	long int tempo_thr;
	tempo_thr = thread_info[i].start.tv_sec*BILLION + thread_info[i].start.tv_nsec;

	/* define a prioridade da thread */
  priorities(i);

	/* Ciclo que executa a thread até o tempo definido para a mesma terminar */
  for(;tempo_thr < (thread_info[i].end.tv_sec*BILLION+thread_info[i].end.tv_nsec);){

		/* A thread adormece até o tempo definido */
		new_rt_task_wait_period(i);

    f3(2, 5);

		/* Calculo do tempo de computação da thread, através da diferença
		 * entre a hora actual do sistema com o tempo quando a thread
		 * iniciou a sua execução */
    Dados_thr[i].tempo_comp =  hora_sistema() - tempo_thr;

		/* Valores usados para fazer a impressão das informações das threads
		 * onde uma delas identifica a thread e a outra recebe o valor do tempo
		 * de computação */
		Imprimir.indice_tab++;
		Imprimir.id_thr_imprimir[Imprimir.indice_tab] = 3;
		Imprimir.t_comp_imprimir[Imprimir.indice_tab] = Dados_thr[i].tempo_comp/CLOCKS_PER_SEC;

		/* Variável usada para calcular a percentagem do numero de vezes que a
		 * thread cumpriu, ou nao, a meta. Ou seja, para calcular a percentagem
		 * de sucesso tarefa */
		Dados_thr[i].num_real_execucao++;

    /* É incrementado o num_execucao caso a thread não consiga atingir a meta.
		 * Esse valor é usado para calcular a percentagem de sucesso tarefa */
    if(Dados_thr[i].tempo_comp < thread_info[i].period.tv_nsec){
			Dados_thr[i].num_execucao++;
		}

		tempo_thr = thread_info[i].start.tv_sec*BILLION + thread_info[i].start.tv_nsec;

		/* Condição que verifica se o tempo de execução da thread chegou ao Fim
		 * e como também, verifica se a mudança de prioridade
		 * já foi feita ou não */
		if((tempo_thr > (Relogio.t_inv.tv_sec*BILLION+Relogio.t_inv.tv_nsec)) && Dados_thr[i].mudanca_prioridade == 0){

			/* Espera 1 seguando, para que as outras threads terminem */
			struct timespec espera_thr;
			espera_thr.tv_sec = 1;
			espera_thr.tv_nsec = 0;
			clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &espera_thr, NULL);

			/* O calculo da precentagem, antes da mudança é feito aqui */
      Dados_thr[i].percentagem[0] = 100*Dados_thr[i].num_execucao/Dados_thr[i].num_real_execucao;

			/* Re-definição das variaveis */
			Dados_thr[i].num_real_execucao = 0;
			Dados_thr[i].num_execucao = 0;

			/* Variável usada para confirmar que a mudança de prioridade
			 * já foi feita, logo não é necessário entrar de novo no if */
			Dados_thr[i].mudanca_prioridade = 1;

			/* Condição usada para verificar se já foi armazendo
			 * o valor -5 na tabela por outra thread, e caso não
			 * se confirme, então armazena o valor na tabela */
			if(Imprimir.confirma_mud_pri == -1){
				/* Valor usado para garantir que deve imprimir que foi feita
				 * feita a mudança de variável */
				Imprimir.indice_tab++;
				Imprimir.id_thr_imprimir[Imprimir.indice_tab] = -5;

				/* Já foi armazenado o valor, e por isso, mais nenhuma
				 * thread o pode fazer */
				Imprimir.confirma_mud_pri = 0;

				/* Inicio = indica que as threads só vão iniciar 2 segundos depois
				 * da hora obtida do sistema(com clock_gettime)  */
				clock_gettime(CLOCK_MONOTONIC,&Relogio.inicio);
				Relogio.inicio.tv_sec += 2;
			}

			/* Introdução dos dados referente a thred 1 */
			new_rt_task_make_periodic(i, 99, Relogio.inicio, Dados_thr[i].periodo, 2);

			tempo_thr = thread_info[i].start.tv_sec*BILLION + thread_info[i].start.tv_nsec;

			/* define a prioridade da thread */
		  priorities(i);
		}
  }

	/* O calculo da precentagem é feito aqui */
  Dados_thr[i].percentagem[1] = 100*Dados_thr[i].num_execucao/Dados_thr[i].num_real_execucao;
  /* faz exit da thread */
  pthread_exit(NULL);
}

/* Funcçao usada para imprimir as informações que foram armazenadas
 * durante a execução das threads */
void imprimir(){
	int i;
	for(i=0 ; i <= Imprimir.indice_tab ; ++i ){
		if(Imprimir.id_thr_imprimir[i] == -5){
			printf("\nPercentagem de sucesso da Tarefa 1: %d%%\n",Dados_thr[0].percentagem[0]);
			printf("Percentagem de sucesso da Tarefa 2: %d%%\n",Dados_thr[1].percentagem[0]);
			printf("Percentagem de sucesso da Tarefa 3: %d%%\n\n",Dados_thr[2].percentagem[0]);
		}
		else{
			printf("Tarefa %d: %ld \tms\n",Imprimir.id_thr_imprimir[i],Imprimir.t_comp_imprimir[i]);
		}
	}

	printf("\nPercentagem de sucesso da Tarefa 1: %d%%\n",Dados_thr[0].percentagem[1]);
	printf("Percentagem de sucesso da Tarefa 2: %d%%\n",Dados_thr[1].percentagem[1]);
	printf("Percentagem de sucesso da Tarefa 3: %d%%\n",Dados_thr[2].percentagem[1]);
}
