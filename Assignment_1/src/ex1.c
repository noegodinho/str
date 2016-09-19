#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "../lib/func.h"

/* PL2, G5 */

int main(){
    cpu_set_t set;

    /* clear cpu mask */
    CPU_ZERO(&set);
    /* set cpu 0 */ 
    CPU_SET(0, &set);

    /* 0 is the calling process */
    if(sched_setaffinity(0, sizeof(cpu_set_t), &set) == -1){
        perror("Error from sched_setaffinity");
    }
}
