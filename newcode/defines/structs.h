#ifndef ST
#define ST



typedef struct {
    int pid;
    int state;  // 0 = ready; 1 = executing; 2 = blocked; 3 = finished;
    int live_time;
} pcb_t;

// -- MACHINE --

typedef struct {
    pthread_t pth_id;
    int id;
    int core_id;
    int cpu_id;
} Thread;

typedef struct {
    int id;
    int num_threads;
    Thread* threads;
} Core;

typedef struct {
    int id;
    int num_cores;
    Core* cores;
} CPU;

typedef struct {
    CPU* cpus;
    int num_cpus;
} machine_t;

// -- MACHINE --

typedef struct {
    int freq_pgen[2];
    int freq_schl;
    int done;
    int finished;
    machine_t machine;
    // int num_timers;
} args_t;

#endif