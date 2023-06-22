#ifndef ST
#define ST

/* -- QUEUE -- */
enum Status {
    READY,
    EXECUTING,
    BLOCKED,
    FINISHED,
    NULL_S
};

typedef struct {
    int pid;
    enum Status status;
    int load_quantum;
    int quantum;
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
    pcb_t pcb;
} thread_t;

typedef struct {
    int id;
    int num_threads;
    thread_t* threads;
    process_queue queue;
    int num_proc_queue;
} core_t;

typedef struct {
    int id;
    int num_cores;
    core_t* cores;
} cpu_t;

typedef struct {
    cpu_t* cpus;
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