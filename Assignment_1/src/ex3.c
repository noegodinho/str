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

/* ms = milisegundos
 * Nos comentários tarefas = threads */

/* Estrutura com o tempo de execução das tarefas */
struct tempo_execucao{
	long int Inicio,Fim;
}relogio;

/* Estrutura usada pelas threads para armazenar e usar informações importantes */
struct Dados_thread{
	/* Variaveis usadas para calcular a percentagem de sucesso de execuções
	 * das threads */
	int num_real_execucao;
	int num_execucao;
	int Percentagem;

	/* Variaveis para o tempo de execução das threads como também o tempo
	 * de computação */
	long int tempo_execucao;
	long int tempo_comp;
}Dados[3];

/* Definiçoes das Funções */
void *func1(void *arg);
void *func2(void *arg);
void *func3(void *arg);
void priorities(int);
void sleep_thr(long int);
long int hora_sistema_ms();
void imprimir();

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

  /* Inicio = indica que as threads só vão iniciar
   * 2 segundos depois da hora obtida do sistema,
   * e terminam 2 segundos depois de terem iniciado */
  relogio.Inicio = hora_sistema_ms() + 2000;
  relogio.Fim = relogio.Inicio + 2000;

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
	/* define a prioridade da thread */
  priorities(99);

	/* Inicialização das variaveis */
  Dados[0].num_real_execucao=0;
  Dados[0].num_execucao=0;
	/* Definição do tempo onde as threads devem começar */
  Dados[0].tempo_execucao = relogio.Inicio;

	/* Ciclo que executa a thread até o tempo definido para a mesma terminar */
  for(; Dados[0].tempo_execucao < relogio.Fim ;){
		/* A thread adormece até o tempo definido */
		sleep_thr(Dados[0].tempo_execucao);

    f1(2, 5);

		/* Calculo do tempo de computação da thread, através da diferença
		 * entre a hora actual do sistema com o tempo quando a thread
		 * iniciou a sua execução */
    Dados[0].tempo_comp =  hora_sistema_ms() - Dados[0].tempo_execucao;

		/* Valores usados para fazer a impressão das informações das threads
		 * onde uma delas identifica a thread e a outra recebe o valor do tempo
		 * de computação */
		indice_a_ser_incrementado++;
		id_thr_imprimir[indice_a_ser_incrementado] = 1;
		tempo_comp_thread_imprimir[indice_a_ser_incrementado] = Dados[0].tempo_comp;

		/* Variável usada para calcular a percentagem do numero de vezes que a
		 * thread cumpriu, ou nao, a meta. Ou seja, para calcular a percentagem
		 * de sucesso tarefa */
    Dados[0].num_real_execucao++;

    /* É incrementado o num_execucao caso a thread não consiga atingir a meta.
		 * Esse valor é usado para calcular a percentagem de sucesso tarefa */
    if(Dados[0].tempo_comp < P1_Activacao) Dados[0].num_execucao++;

    /* Calculo do proximo período de activação da tarefa */
    Dados[0].tempo_execucao += P1_Activacao;
  }

	/* Espera que todas as threads terminem */
  sleep_thr(Dados[0].tempo_execucao+100);

	/* O calculo da precentagem é feito aqui */
  Dados[0].Percentagem = 100*Dados[0].num_execucao/Dados[0].num_real_execucao;
	/* faz exit da thread */
  pthread_exit(NULL);
}

/* Função para thread 2 */
void *func2(void *arg){
	/* define a prioridade da thread */
  priorities(98);

	/* Inicialização das variaveis */
  Dados[1].num_real_execucao=0;
  Dados[1].num_execucao=0;
	/* Definição do tempo onde as threads devem começar */
  Dados[1].tempo_execucao = relogio.Inicio;

	/* Ciclo que executa a thread até o tempo definido para a mesma terminar */
  for(; Dados[1].tempo_execucao < relogio.Fim ;){
		/* A thread adormece até o tempo definido */
		sleep_thr(Dados[1].tempo_execucao);

    f2(2, 5);

		/* Calculo do tempo de computação da thread, através da diferença
		 * entre a hora actual do sistema com o tempo quando a thread
		 * iniciou a sua execução */
    Dados[1].tempo_comp =  hora_sistema_ms() - Dados[1].tempo_execucao;

		/* Valores usados para fazer a impressão das informações das threads
		 * onde uma delas identifica a thread e a outra recebe o valor do tempo
		 * de computação */
		indice_a_ser_incrementado++;
		id_thr_imprimir[indice_a_ser_incrementado] = 2;
		tempo_comp_thread_imprimir[indice_a_ser_incrementado] = Dados[1].tempo_comp;

		/* Variável usada para calcular a percentagem do numero de vezes que a
		 * thread cumpriu, ou nao, a meta. Ou seja, para calcular a percentagem
		 * de sucesso tarefa */
    Dados[1].num_real_execucao++;

		/* É incrementado o num_execucao caso a thread não consiga atingir a meta.
		 * Esse valor é usado para calcular a percentagem de sucesso tarefa */
    if(Dados[1].tempo_comp < P2_Activacao) Dados[1].num_execucao++;

    /* Calculo do proximo período de activação da tarefa */
    Dados[1].tempo_execucao += P2_Activacao;
  }

	/* Espera que todas as threads terminem */
  sleep_thr(Dados[1].tempo_execucao+100);

	/* O calculo da precentagem é feito aqui */
  Dados[1].Percentagem = 100*Dados[1].num_execucao/Dados[1].num_real_execucao;
  /* faz exit da thread */
  pthread_exit(NULL);
}

/* Função para thread 3 */
void *func3(void *arg){
	/* define a prioridade da thread */
  priorities(97);

	/* Inicialização das variaveis */
  Dados[2].num_real_execucao=0;
  Dados[2].num_execucao=0;
	/* Definição do tempo onde as threads devem começar */
  Dados[2].tempo_execucao = relogio.Inicio;

	/* Ciclo que executa a thread até o tempo definido para a mesma terminar */
  for(; Dados[2].tempo_execucao < relogio.Fim ;){
		/* A thread adormece até o tempo definido */
  	sleep_thr(Dados[2].tempo_execucao);

    f3(2, 5);

		/* Calculo do tempo de computação da thread, através da diferença
		 * entre a hora actual do sistema com o tempo quando a thread
		 * iniciou a sua execução */
    Dados[2].tempo_comp =  hora_sistema_ms() - Dados[2].tempo_execucao;

		/* Valores usados para fazer a impressão das informações das threads
		 * onde uma delas identifica a thread e a outra recebe o valor do tempo
		 * de computação */
		indice_a_ser_incrementado++;
		id_thr_imprimir[indice_a_ser_incrementado] = 3;
		tempo_comp_thread_imprimir[indice_a_ser_incrementado] = Dados[2].tempo_comp;

		/* Variável usada para calcular a percentagem do numero de vezes que a
		 * thread cumpriu, ou nao, a meta. Ou seja, para calcular a percentagem
		 * de sucesso tarefa */
    Dados[2].num_real_execucao++;

		/* É incrementado o num_execucao caso a thread não consiga atingir a meta.
		 * Esse valor é usado para calcular a percentagem de sucesso tarefa */
    if(Dados[2].tempo_comp < P3_Activacao) Dados[2].num_execucao++;

    /* Calculo do proximo período de activação da tarefa */
    Dados[2].tempo_execucao += P3_Activacao;
	}

	/* Espera que todas as threads terminem */
  sleep_thr(Dados[2].tempo_execucao+100);

	/* O calculo da precentagem é feito aqui */
  Dados[2].Percentagem = 100*Dados[2].num_execucao/Dados[2].num_real_execucao;
  /* faz exit da thread */
  pthread_exit(NULL);
}

/* Função que define o valor das prioridades das threads */
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

/* Funão que recebe a hora do sistema com clock_gettime e
 * devolve o resultado em milisegundos */
long int hora_sistema_ms(){
	long int ms,precisao_ms;

	struct timespec tempo_actual;
	clock_gettime(CLOCK_MONOTONIC,&tempo_actual);

	/* Converte para milisegundos.
	 * 1ns=1*10^-9, para milisegundos fica (1*10^-9)*(1*10^6)=1*10^-3.
	 * Como os valores estão no formato inteiro long com nove casas, então
	 * para obtermos as 3 primeiras casas, que representam os mili temos
	 * que (1*10^9)*(1*10^-6)=1*10^3, ou seja, se 100000000 = ns no formato long
	 * e aplicarmos a seguinte expressão 100000000 * 10^-6 = 100.
	 * 100 = ms no formato long */
	ms = tempo_actual.tv_nsec/1e6;

	/* ## Conversão de segundos para milisegundos, 1 seg = 1*10^3 ms
	 * É adicionado os milisegundos ao segundos para ser mais preciso
	 * Exemplo: 1*1e3=1000, então o tempo será 1+milisegundos que ficam nas
	 * 3 ultimas casas */
	precisao_ms=(tempo_actual.tv_sec*1e3)+ms;

	return precisao_ms;
}

/* Funão que recebe que as threads vão adormecer e
 * e com o clock_nanosleep() */
void sleep_thr(long int times){
	struct timespec t;
	/* Converte para segundos */
	t.tv_sec=times/(1e3);

	/* Converte para nanosegundos
	 * Nesta linha, toma-se as 3 ultimas casas, através do resto da divisão,
	 * que vêm dos milisegundos calculados na linha que contém ##, e depois
	 * é convertido para nanosegundos. Como para converter para ms dividi
	 * ns/1e6, então para converter novamente para nanosegundos é só multiplicar
	 * por 1e6 */
	t.tv_nsec=(times%1000)*(1e6);

	/* Adormece a thread */
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
}

/* Funcçao usada para imprimir as informações que foram armazenadas
 * durante a execução das threads */
void imprimir(){
	for(int i=0 ; i <= indice_a_ser_incrementado ; ++i ){
		printf("Tarefa %d: %ld \tms\n",id_thr_imprimir[i],tempo_comp_thread_imprimir[i]);
	}

	printf("\nPercentagem de sucesso da Tarefa 1: %d%%\n",Dados[0].Percentagem);
	printf("Percentagem de sucesso da Tarefa 2: %d%%\n",Dados[1].Percentagem);
	printf("Percentagem de sucesso da Tarefa 3: %d%%\n",Dados[2].Percentagem);
}
