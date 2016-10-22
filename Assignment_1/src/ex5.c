/****************************************************************
 * Autor: José Manuel C. Noronha
 * Autor: Noé Godinho
 * Turma: PL2
 * Grupo: 5
 * Ano Lectivo: 2016 - 2017
 ***************************************************************/

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include "../lib/func.h"
#define BILLION 1e9

void calculate(double t, char *name);

/* função que imita f1 de func.o */
void f1(int n, int m){
    calculate(0.033, "f1");
}

/* função que imita f2 de func.o */
void f2(int n, int m){
    calculate(0.053, "f2");
}

/* função que imita f3 de func.o */
void f3(int n, int m){
    calculate(0.083, "f3");
}

void calculate(double t, char *name){
    double t_atual = 0;
    uint64_t t_nano = 0;
    struct timespec start, end;

    /* enquanto o tempo obtido até ao momento for inferior ao tempo pretendido */
    while(t_atual < t){
        /* começa o cálculo do tempo */
        clock_gettime(CLOCK_MONOTONIC, &start);

        /* realiza umas iterações para ocupar CPU */
        for(int i = 0; i < 100000; ++i){
            ;
        }

        /* termina o cálculo do tempo */
        clock_gettime(CLOCK_MONOTONIC, &end);

        /* converte para nanosegundos os segundos e soma-os aos nanosegundos da estrutura */
        t_nano += (BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec);
        /* converte o obtido para segundos */
        t_atual = (double)t_nano / BILLION;
    }
}
