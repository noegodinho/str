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
#include <mqueue.h>

#define PI 3.141592
#define BILLION 1e9
#define NUM_THREADS 3
#define N 3
#define CGeracao 73

#define QUEUE_NAME "/queue"
#define QUEUE_NAME2 "/queue2"
#define QUEUE_NAME3 "/queue3"

#define CHECK(x) \
    do { \
        if (!(x)) { \
            fprintf(stderr, "%s:%d: ", __func__, __LINE__); \
            perror(#x); \
            exit(-1); \
        } \
    } while (0) \

struct Relogio{
    struct timespec start_time;
    struct timespec end_time;
}relogio;

struct wave_info{
    double amplitude;
    double frequency;
    double phase;
};

struct thread_info{
    struct wave_info *wave;
    int number_of_waves;
    int wave_type;
}thread_info;

struct Signal_Info{
    struct timespec periodo;
    struct timespec tempo_execucao;
}signal_info[NUM_THREADS];


void start_thread_time();
void priorities(int);
void sleep_thread(time_t, long);
long int hora_sistema();
void dfour1(double [], unsigned long, int);
void a_corr(double *, int, double *);
double sinusoidal_wave(long , int);
double triangular_wave(long , int);
double square_wave(long , int);

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

    FILE *gnuplotPipe = popen("gnuplot -persistent", "w");

    fprintf(gnuplotPipe, "%s\n", "set multiplot layout 3,1 rowsfirst");
    fprintf(gnuplotPipe, "%s\n", "plot 'wave.dat' with lines title \" Onda original\"");
    fprintf(gnuplotPipe, "%s\n", "plot 'fft.dat' with lines title \" FFT\"");
    fprintf(gnuplotPipe, "%s\n", "plot 'auto.dat' with lines title \" Auto correlação\"");

    fclose(gnuplotPipe);

    mq_unlink(QUEUE_NAME);
    mq_unlink(QUEUE_NAME2);
    mq_unlink(QUEUE_NAME3);

    return 0;
}

void start_thread_time(){
    struct timespec time_struct;
    clock_gettime(CLOCK_MONOTONIC, &time_struct);

    /* Definimos que as threads devem iniciar 2 segundos depois da hora actual do sistema */
    relogio.start_time.tv_sec = time_struct.tv_sec + 2;
    relogio.start_time.tv_nsec = time_struct.tv_nsec;

    /* Definimos que as threads devem terminar 2 segundos depois de terem iniciado */
    relogio.end_time.tv_sec = relogio.start_time.tv_sec + 1;
    relogio.end_time.tv_nsec = 0;
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
    long time_var;
    long actual_time, end_time;
    double onda_valor[N], onda[N];
    int j, i, flag = 0;
    struct mq_attr attr;
    mqd_t mq, mq2, mq3;
    struct timespec tp;

    j = 0;
    i = 0;
    signal_info[i].periodo.tv_sec = 0;
    signal_info[i].periodo.tv_nsec = 110*CGeracao;
    signal_info[i].tempo_execucao.tv_sec = relogio.start_time.tv_sec;
    signal_info[i].tempo_execucao.tv_nsec = relogio.start_time.tv_nsec;

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(double) * N;
    attr.mq_curmsgs = 0;

    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0644, &attr);
    CHECK((mqd_t)-1 != mq);

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(double) * N;
    attr.mq_curmsgs = 0;

    mq2 = mq_open(QUEUE_NAME2, O_CREAT | O_RDWR, 0644, &attr);
    CHECK((mqd_t)-1 != mq);

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(double) * N;
    attr.mq_curmsgs = 0;

    mq3 = mq_open(QUEUE_NAME3, O_CREAT | O_RDWR, 0644, &attr);
    CHECK((mqd_t)-1 != mq);

    priorities(99);    

    FILE *save_values = fopen("wave.dat", "w");

    actual_time = signal_info[i].tempo_execucao.tv_sec * BILLION + signal_info[i].tempo_execucao.tv_nsec;
    end_time = relogio.end_time.tv_sec * BILLION + relogio.end_time.tv_nsec;

    while(actual_time < end_time){
        sleep_thread(signal_info[i].tempo_execucao.tv_sec, signal_info[i].tempo_execucao.tv_nsec);        

        if(flag == 1){
            clock_gettime(CLOCK_MONOTONIC, &tp);
            tp.tv_sec += 100;
            mq_timedreceive(mq2, (char *) &onda, sizeof(double) * N, NULL, &tp);
            clock_gettime(CLOCK_MONOTONIC, &tp);
            tp.tv_sec += 100;
            mq_timedreceive(mq2, (char *) &onda, sizeof(double) * N, NULL, &tp);
            flag = 0;
        }

        time_var = hora_sistema();

        if(thread_info.wave_type == 0)
            onda_valor[j] = sinusoidal_wave(time_var, j);
        else if(thread_info.wave_type == 1)
            onda_valor[j] = triangular_wave(time_var, j);
        else
            onda_valor[j] = square_wave(time_var, j);        
        
        signal_info[i].tempo_execucao.tv_sec += signal_info[i].periodo.tv_sec;
        signal_info[i].tempo_execucao.tv_nsec += signal_info[i].periodo.tv_nsec;

        if(signal_info[i].tempo_execucao.tv_nsec > BILLION){
            signal_info[i].tempo_execucao.tv_nsec -= BILLION;
            ++signal_info[i].tempo_execucao.tv_sec;
        }

        actual_time = signal_info[i].tempo_execucao.tv_sec * BILLION + signal_info[i].tempo_execucao.tv_nsec;

        fprintf(save_values, "%ld %lf\n", time_var, onda_valor[j]);               

        if(j == N-1){
            j = 0;
            clock_gettime(CLOCK_MONOTONIC, &tp);
            tp.tv_sec += 100;
            mq_timedsend(mq, (const char *) &onda_valor, sizeof(double) * N, 0, &tp);
            clock_gettime(CLOCK_MONOTONIC, &tp);
            tp.tv_sec += 100;
            mq_timedsend(mq3, (const char *) &onda_valor, sizeof(double) * N, 0, &tp);
            flag = 1;
        }

        ++j;
    }

    //fclose(save_values);

    mq_close(mq);
    mq_close(mq2);
    mq_close(mq3);

    pthread_exit(NULL);
}

double sinusoidal_wave(long time_var, int j){
    double omega, phase, sum;

    sum = 0;
    for(int i = 0; i < thread_info.number_of_waves; ++i){
        omega = 2 * PI * thread_info.wave[i].frequency;
        phase = thread_info.wave[i].phase * PI / 180.0;        
        
        sum += (thread_info.wave[i].amplitude * sin(omega * time_var / BILLION + phase));
    }

    // O resultado do sinal no instante é armazenado no array
    return sum;
}

double triangular_wave(long time_var, int i){
    double m, value_to_return;
    long phase_time, period;

    period = BILLION / thread_info.wave->frequency;
    phase_time = fmod((thread_info.wave->phase * period) / (2*PI), period);
    m = thread_info.wave->amplitude * 4 / period;

    time_var = fmod(time_var + period - phase_time, period);

    if(time_var >= 0 && time_var < period / 4){
        value_to_return = m * time_var;        
    }

    else if(time_var >= period / 4 && time_var < 3 * period / 4){            
        value_to_return = m * (period / 2 - time_var);
    }

    else{            
        value_to_return = m * (time_var - period);
    }

    return value_to_return;
}

double square_wave(long time_var, int i){
    long phase_time, period;
    double value_to_return;

    period = BILLION / thread_info.wave->frequency;
    phase_time = fmod((thread_info.wave->phase * period) / (2*PI), period);

    time_var = fmod(time_var + period - phase_time, period);

    if(time_var >= 0 && time_var < period / 2){
        value_to_return = thread_info.wave->amplitude;        
    }

    else{            
        value_to_return = -thread_info.wave->amplitude;
    }

    return value_to_return;
}

void *fft(void *arg){
    double *X;
    double actual_time, end_time;
    double onda[N];
    double sqrt_calc;
    int i = 1;
    mqd_t mq, mq2;
    struct mq_attr attr;
    struct timespec tp;

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(double) * N;
    attr.mq_curmsgs = 0;

    X = (double *)malloc(sizeof(double) * (2*N));
    signal_info[i].periodo.tv_sec = 0;
    signal_info[i].periodo.tv_nsec = 4*110*CGeracao;
    signal_info[i].tempo_execucao.tv_sec = relogio.start_time.tv_sec;
    signal_info[i].tempo_execucao.tv_nsec = relogio.start_time.tv_nsec;    

    mq = mq_open(QUEUE_NAME, O_RDWR | O_CREAT, 0644, &attr);
    CHECK((mqd_t)-1 != mq);

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(double) * N;
    attr.mq_curmsgs = 0;

    mq2 = mq_open(QUEUE_NAME2, O_CREAT | O_RDWR, 0644, &attr);
    CHECK((mqd_t)-1 != mq);

    priorities(98);    

    actual_time = signal_info[i].tempo_execucao.tv_sec * BILLION + signal_info[i].tempo_execucao.tv_nsec;
    end_time = relogio.end_time.tv_sec * BILLION + relogio.end_time.tv_nsec;

    FILE *save_values = fopen("fft.dat", "w");

    while(actual_time < end_time){
        sleep_thread(signal_info[i].tempo_execucao.tv_sec, signal_info[i].tempo_execucao.tv_nsec);
        clock_gettime(CLOCK_MONOTONIC, &tp);
        tp.tv_sec += 100;
        mq_timedreceive(mq, (char *) &onda, sizeof(double) * N, NULL, &tp);

        for(int i = 0, j = 0; i < 2*N; i += 2, ++j){
            X[i] = onda[j];
            X[i+1] = 0.0;
        }

        dfour1(X-1, N, 1);

        for(int i = 0; i < 2*N; i+=2){
            sqrt_calc = sqrt(X[i]*X[i] + X[i+1]*X[i+1]);

            fprintf(save_values, "%ld %lf\n", hora_sistema(), sqrt_calc);                
        }

        signal_info[i].tempo_execucao.tv_sec += signal_info[i].periodo.tv_sec;
        signal_info[i].tempo_execucao.tv_nsec += signal_info[i].periodo.tv_nsec;

        if(signal_info[i].tempo_execucao.tv_nsec > BILLION){
            signal_info[i].tempo_execucao.tv_nsec -= BILLION;
            ++signal_info[i].tempo_execucao.tv_sec;
        }        

        actual_time = signal_info[i].tempo_execucao.tv_sec * BILLION + signal_info[i].tempo_execucao.tv_nsec;
        clock_gettime(CLOCK_MONOTONIC, &tp);
        tp.tv_sec += 100;
        mq_timedsend(mq2, (const char *) &onda, sizeof(double) * N, 0, &tp);        
    }

    //fclose(save_values);

    mq_close(mq);
    mq_close(mq2);

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
    double Rxx[N+1];
    double actual_time, end_time;
    double onda2[N];
    mqd_t mq, mq2;
    struct mq_attr attr;
    struct timespec tp;
    int i = 2;

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(double) * N;
    attr.mq_curmsgs = 0;

    signal_info[i].periodo.tv_sec = 0;
    signal_info[i].periodo.tv_nsec = 11*110*CGeracao;
    signal_info[i].tempo_execucao.tv_sec = relogio.start_time.tv_sec;
    signal_info[i].tempo_execucao.tv_nsec = relogio.start_time.tv_nsec;

    mq = mq_open(QUEUE_NAME3, O_RDWR | O_CREAT, 0644, &attr);
    CHECK((mqd_t)-1 != mq);

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(double) * N;
    attr.mq_curmsgs = 0;

    mq2 = mq_open(QUEUE_NAME2, O_CREAT | O_RDWR, 0644, &attr);
    CHECK((mqd_t)-1 != mq);

    priorities(97);

    actual_time = signal_info[i].tempo_execucao.tv_sec * BILLION + signal_info[i].tempo_execucao.tv_nsec;
    end_time = relogio.end_time.tv_sec * BILLION + relogio.end_time.tv_nsec;

    FILE *save_values = fopen("auto.dat", "w");

    while(actual_time < end_time){
        sleep_thread(signal_info[i].tempo_execucao.tv_sec, signal_info[i].tempo_execucao.tv_nsec);
        clock_gettime(CLOCK_MONOTONIC, &tp);
        tp.tv_sec += 100;     
        mq_timedreceive(mq, (char *) &onda2, sizeof(double) * N, NULL, &tp);

        a_corr(Rxx, pos_meio, onda2);

        for(int k = 0; k < N; ++k){            
            fprintf(save_values, "%ld %lf\n", hora_sistema(), Rxx[k]);                             
        }      

        signal_info[i].tempo_execucao.tv_sec += signal_info[i].periodo.tv_sec;
        signal_info[i].tempo_execucao.tv_nsec += signal_info[i].periodo.tv_nsec;

        if(signal_info[i].tempo_execucao.tv_nsec > BILLION){
            signal_info[i].tempo_execucao.tv_nsec -= BILLION;
            ++signal_info[i].tempo_execucao.tv_sec;
        }        

        actual_time = signal_info[i].tempo_execucao.tv_sec * BILLION + signal_info[i].tempo_execucao.tv_nsec;
        clock_gettime(CLOCK_MONOTONIC, &tp);
        tp.tv_sec += 100;
        mq_timedsend(mq2, (const char *) &onda2, sizeof(double) * N, 0, &tp);
    }

    //fclose(save_values);

    mq_close(mq);
    mq_close(mq2);

    pthread_exit(NULL);
}

void a_corr(double *Rx, int p_meio, double *onda_valor){
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
