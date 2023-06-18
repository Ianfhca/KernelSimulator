#ifndef ST
#define ST

typedef struct {
    int freq_pgen;
    int freq_schl;
    int done;
    int finished;
    // int num_timers;
} args_t;

typedef struct {
    int num_CPUs;
    int num_cores;
    int num_threads;
} machine_t;

typedef struct {
    int pid;
    int state;  // 0 = ready; 1 = executing; 2 = blocked; 3 = finished;
} pcb_t;

#endif