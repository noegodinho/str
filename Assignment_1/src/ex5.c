#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include "../lib/func.h"
#define BILLION 1e9

void calculate(double t, char *name);

void f1(int n, int m){
    calculate(0.033, "f1");
}

void f2(int n, int m){
    calculate(0.053, "f2");
}

void f3(int n, int m){
    calculate(0.083, "f3");
}

void calculate(double t, char *name){
    double t_atual;
    uint64_t t_nano = 0;
    struct timespec start, end;

    while(1){
        clock_gettime(CLOCK_MONOTONIC, &start);
        
        for(int i = 0; i < 100000; ++i){
            ;
        }

        clock_gettime(CLOCK_MONOTONIC, &end);

        t_nano += (BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec);
        t_atual = (double)t_nano / BILLION;

        if(t_atual > t){
            break;
        }
    }
}
