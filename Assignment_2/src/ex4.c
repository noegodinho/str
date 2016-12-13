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
#define NUM_THREADS 3
#define N 1024

/* Array com os valores das ondes geradas */
double onda_valor[N], onda_valor_parte_im[N];


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

    /* Variáveis que contem o segundo
     * e o nanosegundo da thread da fft */
    time_t start_fft_autocorr_seconds;
    long start_fft_autocorr_nseconds;
}thread_info;


void start_thread_time();
void priorities(int);
void sleep_thread(time_t, long);
long int hora_sistema();
void dfour1(double [], unsigned long, int);
void a_corr(double *, int);
void sinusoidal_wave(long , int);
void triangular_wave(long , int);
void square_wave(long , int);

void *gera_sinal(void *);
void *fft(void *);
void *auto_correlacao(void *);

int main(int argc, char **argv){
    cpu_set_t set;
    pthread_t thread[NUM_THREADS];
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

    // Inicializo a parte imaginaria da onda
    for(i = 0; i < N; ++i){
        onda_valor_parte_im[i] = 0.0;
    }

    start_thread_time();

    pthread_create(&thread[0], NULL, &gera_sinal, NULL);

    pthread_create(&thread[1], NULL, &fft, NULL);

    pthread_create(&thread[2], NULL, &auto_correlacao, NULL);

    for(int i = 0; i < NUM_THREADS; ++i){
        pthread_join(thread[i], NULL);
    }
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

    /* Estou a definir que o thread para fft deve ser activada
     * 1 segundo depois da thread que geradora de sinal, visto que
     * a fft depende dos dados provenientes da geração */
    thread_info.start_fft_autocorr_seconds = thread_info.start_time_seconds + 1;
    thread_info.start_fft_autocorr_nseconds = 0;
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

/* Função usada pelas threads, com o objectivo de receber a hora
 * do sistema com clock_gettime e devolver esse mesmo tempo */
long int hora_sistema(){
	struct timespec tempo_actual;
	clock_gettime(CLOCK_MONOTONIC,&tempo_actual);

	return (tempo_actual.tv_sec*BILLION + tempo_actual.tv_nsec);
}

void sleep_thread(time_t sec, long nsec){
    struct timespec time_to_sleep;
    time_to_sleep.tv_sec = sec;
    time_to_sleep.tv_nsec = nsec;

    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &time_to_sleep, NULL);
}

void *gera_sinal(void *arg){
    long time_var, average;
    int j;

    priorities(99);
    sleep_thread(thread_info.start_time_seconds, thread_info.start_time_nanoseconds);

    average = 0;
    for(j = 0; j < N; ++j){
        time_var = hora_sistema();
        if(thread_info.wave_type == 0)
            sinusoidal_wave(time_var, j);
        else if(thread_info.wave_type == 1)
            triangular_wave(time_var, j);
        else
            square_wave(time_var, j);        

        average += hora_sistema() - time_var;
    }
    printf("Geracao\t\t=\t%ld\tnano-segundos\n",average/N);

    pthread_exit(NULL);
}

void sinusoidal_wave(long time_var, int j){
    double omega, phase, sum;

    sum = 0;
    for(int i = 0; i < thread_info.number_of_waves; ++i){
        omega = 2 * PI * thread_info.wave[i].frequency;
        phase = thread_info.wave[i].phase * PI / 180.0;        
        
        sum += (thread_info.wave[i].amplitude * sin(omega * time_var / BILLION + phase));
    }

    // O resultado do sinal no instante é armazenado no array
    onda_valor[j] = sum;
}

void triangular_wave(long time_var, int i){
    double m;
    long phase_time, period;

    period = BILLION / thread_info.wave->frequency;
    phase_time = fmod((thread_info.wave->phase * period) / (2*PI), period);
    m = thread_info.wave->amplitude * 4 / period;

    time_var = fmod(time_var + period - phase_time, period);

    if(time_var >= 0 && time_var < period / 4){
        onda_valor[i] = m * time_var;        
    }

    else if(time_var >= period / 4 && time_var < 3 * period / 4){            
        onda_valor[i] = m * (period / 2 - time_var);
    }

    else{            
        onda_valor[i] = m * (time_var - period);
    }
}

void square_wave(long time_var, int i){
    long phase_time, period;

    period = BILLION / thread_info.wave->frequency;
    phase_time = fmod((thread_info.wave->phase * period) / (2*PI), period);

    time_var = fmod(time_var + period - phase_time, period);

    if(time_var >= 0 && time_var < period / 2){
        onda_valor[i] = thread_info.wave->amplitude;        
    }

    else{            
        onda_valor[i] = -thread_info.wave->amplitude;
    }
}

void *fft(void *arg){
    double *X;
    X = (double *)malloc(sizeof(double) * (2*N));

    long int average, time_actual;

    priorities(99);
    sleep_thread(thread_info.start_fft_autocorr_seconds, thread_info.start_fft_autocorr_nseconds);

    /* Aplico a expressão da FFT unidimensional, calculos depois
     * apresentados no relatório, a explicar como chegamos a essas
     * expressões aqui aplicadas */
    average = 0;
    for(int t = 0; t < N; ++t){
        time_actual = hora_sistema();
        for(int i = 0, j = 0; i < 2*N; i += 2, ++j){
            X[i] = onda_valor[j];
            X[i+1] = 0.0;
        }

        dfour1(X-1, N, 1);

        average += hora_sistema() - time_actual;
    }
    printf("FFT\t\t=\t%ld\tnano-segundos\n",average/N);
    pthread_exit(NULL);
}

#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr

void dfour1(double data[], unsigned long nn, int isign){
    unsigned long n,mmax,m,j,istep,i;
    double wtemp,wr,wpr,wpi,wi,theta;
    double tempr,tempi;

    n=nn << 1;
    j=1;
    for (i=1;i<n;i+=2) {
        if (j > i) {
            SWAP(data[j],data[i]);
            SWAP(data[j+1],data[i+1]);
        }
        m=n >> 1;
        while (m >= 2 && j > m) {
            j -= m;
            m >>= 1;
        }
        j += m;
    }
    mmax=2;
    while (n > mmax) {
        istep=mmax << 1;
        theta=isign*(6.28318530717959/mmax);
        wtemp=sin(0.5*theta);
        wpr = -2.0*wtemp*wtemp;
        wpi=sin(theta);
        wr=1.0;
        wi=0.0;
        for (m=1;m<mmax;m+=2) {
            for (i=m;i<=n;i+=istep) {
                j=i+mmax;
                tempr=wr*data[j]-wi*data[j+1];
                tempi=wr*data[j+1]+wi*data[j];
                data[j]=data[i]-tempr;
                data[j+1]=data[i+1]-tempi;
                data[i] += tempr;
                data[i+1] += tempi;
            }
            wr=(wtemp=wr)*wpr-wi*wpi+wr;
            wi=wi*wpr+wtemp*wpi+wi;
        }
        mmax=istep;
    }
}

#undef SWAP

void *auto_correlacao(void *arg){
    int pos_meio = N/2;
    long time_actual, average;
	double Rxx[N+1];

	priorities(99);
    sleep_thread(thread_info.start_fft_autocorr_seconds, thread_info.start_fft_autocorr_nseconds);

    average = 0;
    for(int t = 0; t < N; ++t){
        time_actual = hora_sistema();
        a_corr(Rxx, pos_meio);
        average += hora_sistema() - time_actual;
    }
    
    printf("Auto correlacao\t=\t%ld\tnano-segundos\n",average/N);

	pthread_exit(NULL);
}

void a_corr(double *Rx, int p_meio){
    double rxx;

    for(int k = 0; k <= p_meio; ++k){
        rxx = 0.0;
        for(int n = 0; n <= p_meio - k; ++n){
            rxx += onda_valor[n]*onda_valor[n+k];
        }
        Rx[p_meio + k] = rxx;
        Rx[p_meio - k] = rxx;
    }
}