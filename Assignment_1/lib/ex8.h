#define BILLION 1e9

int new_rt_task_make_periodic(int, int, struct timespec, struct timespec, int);
int new_rt_task_make_periodic_relative_ns(int, int, struct timespec, struct timespec, int);
void new_rt_task_wait_period();
