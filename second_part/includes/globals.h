#ifndef GLB
#define GLB

#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "structs.h"

#define NUM_TIMERS 2
#define NUM_THREADS 3
#define MAX_PROCESS 5000
#define DEF_CPUS 2
#define DEF_CORES 2
#define DEF_THREADS 8

pid_t gettid(void);

extern pthread_mutex_t mutex;
extern pthread_cond_t cond1;
extern pthread_cond_t cond2;

extern pthread_mutex_t s_mutex;
extern pthread_cond_t s_cond;

extern int num_cpus, num_cores, num_threads;

extern int process_map[];

extern int exit_process;

#endif