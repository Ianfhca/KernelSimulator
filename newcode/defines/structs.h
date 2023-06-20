#ifndef ST
#define ST

/* -- QUEUE -- */
typedef struct {
    int pid;
    int state;  /* 0 = ready; 1 = executing; 2 = blocked; 3 = finished; */
    int live_time;
    int priority;
} pcb_t;

typedef struct node_t {
    pcb_t pcb;
    struct node_t* next;
} node_t;

typedef struct {
    node_t* first;
    node_t* last;
} process_queue;
/* -- QUEUE -- */

/* -- MACHINE -- */ 
typedef struct {
    /*pthread_t pth_id;*/
    int id;
    int core_id;
    int cpu_id;
} Thread;

typedef struct {
    int id;
    int num_threads;
    Thread* threads;
    process_queue queue;
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
/* -- MACHINE -- */

typedef struct {
    int freq_pgen[2];
    int freq_schl;
    int done;
    int finished;
    machine_t machine;
} args_t;

#endif