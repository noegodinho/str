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
#include <math.h>

#define PI 3.141592
#define BILLION 1e9


struct wave_info{
    double amplitude;
    double frequency;
    double phase;
};

struct thread_info{
    struct wave_info *wave;
    int number_of_waves;
    int wave_type;
    time_t start_time_seconds;
    long start_time_nanoseconds;
}thread_info;


void start_thread_time();
void priorities(int);
void sleep_thread(time_t, long);
void *sinusoidal_wave(void *);
void *triangular_wave(void *);
void *square_wave(void *);


int main(int argc, char **argv){
    cpu_set_t set;
    pthread_t thread;
    int i, scan = 0;

    printf("===========================================================\n");
    printf("================== Iniciando o Programa! ==================\n");
    printf("=========== Sistemas de Tempo Real, PL2, Grupo5 ===========\n");
    printf("===========================================================\n\n");    

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

    /* Tipo de onda */
    if(scan <= 0){
        scan = scanf("%d", &thread_info.wave_type);
    }    

    if(thread_info.wave_type == 0){
        /* Número de ondas */
        scan = scanf("%d", &thread_info.number_of_waves);   
    }

    else{
        thread_info.number_of_waves = 1;
    }

    thread_info.wave = malloc(thread_info.number_of_waves * sizeof(struct wave_info));

    for(i = 0; i < thread_info.number_of_waves; ++i){
        scan = scanf("%lf", &thread_info.wave[i].amplitude);
        scan = scanf("%lf", &thread_info.wave[i].frequency);
        scan = scanf("%lf", &thread_info.wave[i].phase);
    }

    start_thread_time();

    if(thread_info.wave_type == 0){
        pthread_create(&thread, NULL, &sinusoidal_wave, NULL);
    }

    else if(thread_info.wave_type == 1){
        pthread_create(&thread, NULL, &triangular_wave, NULL);
    }

    else{
        pthread_create(&thread, NULL, &square_wave, NULL);
    }

    pthread_join(thread, NULL);
    free(thread_info.wave);

    printf("\n===========================================================\n");
    printf("===================== Fim do Programa! ====================\n");
    printf("===========================================================\n");

    return 0;
}

void start_thread_time(){
    struct timespec time_struct;

    clock_gettime(CLOCK_MONOTONIC, &time_struct);
    thread_info.start_time_seconds = time_struct.tv_sec;
    thread_info.start_time_nanoseconds = time_struct.tv_nsec;
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

void sleep_thread(time_t sec, long nsec){
    struct timespec time_to_sleep;
    time_to_sleep.tv_sec = sec;
    time_to_sleep.tv_nsec = nsec;

    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &time_to_sleep, NULL);
}

void *sinusoidal_wave(void *arg){
    double omega, phase;
    double sum;
    long time_var;
    int i, j;

    priorities(99);
    sleep_thread(thread_info.start_time_seconds, thread_info.start_time_nanoseconds);

    for(j = 0; j <= 10; ++j){
        time_var = j * 1e8;
        sum = 0;

        for(i = 0; i < thread_info.number_of_waves; ++i){
            omega = 2 * PI * thread_info.wave[i].frequency;
            phase = thread_info.wave[i].phase * PI / 180.0;        
            
            sum += (thread_info.wave[i].amplitude * sin(omega * time_var / BILLION + phase));
        }

        printf("Total: %lf, %ld\n", sum, time_var);
    }    

    pthread_exit(NULL);
}

void *triangular_wave(void *arg){
    double m, y;
    long phase_time, period, time_var, time_var2;
    int i;

    priorities(99);
    sleep_thread(thread_info.start_time_seconds, thread_info.start_time_nanoseconds);

    period = BILLION / thread_info.wave->frequency;
    phase_time = fmod((thread_info.wave->phase * period) / (2*PI), period);
    m = thread_info.wave->amplitude * 4 / period;

    for(i = 0; i <= 10; ++i){
        time_var2 = i * 1e8;
        time_var = fmod(time_var2 + period - phase_time, period);

        if(time_var >= 0 && time_var < period / 4){
            y = m * time_var;        
        }

        else if(time_var >= period / 4 && time_var < 3 * period / 4){            
            y = m * (period / 2 - time_var);
        }

        else{            
            y = m * (time_var - period);
        }

        printf("Total: %lf, %ld\n", y, time_var2);
    }

    pthread_exit(NULL);
}

void *square_wave(void *arg){
    double y;
    long phase_time, period, time_var, time_var2;
    int i;

    priorities(99);
    sleep_thread(thread_info.start_time_seconds, thread_info.start_time_nanoseconds);

    period = BILLION / thread_info.wave->frequency;
    phase_time = fmod((thread_info.wave->phase * period) / (2*PI), period);

    for(i = 0; i <= 10; ++i){
        time_var2 = i * 1e8;
        time_var = fmod(time_var2 + period - phase_time, period);

        if(time_var >= 0 && time_var < period / 2){
            y = thread_info.wave->amplitude;        
        }

        else{            
            y = -thread_info.wave->amplitude;
        }

        printf("Total: %lf, %ld\n", y, time_var2);
    }

    pthread_exit(NULL);
}
