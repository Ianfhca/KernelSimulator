#ifndef ST
#define ST

#define MEMORY_SIZE 0xFFFFFF /*2^24 Memory size with a 24-bit address bus*/
#define WORD 4               /*Word size*/

#define KERNEL_MEMORY_SIZE 0x400000 /*2^22 kernel reserved memory address*/

/* -- MEMORY -- */
typedef struct {
    int size;
    int *virtual_address;
    int *physical_address;
} page_t;

typedef struct {
    int *code;
    int *data;
    int *pgb;
} mm_t;

typedef struct {
    int virtual_address[1];
    int physical_address[1];
} tlb_t;

typedef struct {
    tlb_t tlb;
    int *ptbr;
} mmu_t;

typedef struct {
    __uint8_t memory[MEMORY_SIZE];
} physical_memory_t;

typedef struct {
    __uint32_t base_address;
    __uint32_t size;
} mem_region_t;

typedef struct {
    mem_region_t kernel_region;
    mem_region_t user_region;
} mem_layout_t;

/*extern __uint32_t physical_memory[MEMORY_SIZE];*/
/* -- MEMORY -- */

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
    mm_t mm;
} pcb_t;

typedef struct node_t {
    pcb_t pcb;
    struct node_t *next;
} node_t;

typedef struct {
    node_t *first;
    node_t *last;
} process_queue;
/* -- QUEUE -- */

/* -- MACHINE -- */
typedef struct {
    int id;
    int core_id;
    int cpu_id;
    pcb_t pcb;
    mmu_t mmu;
    int *ptbr;
    int ir;
    int pc;
} thread_t;

typedef struct {
    int id;
    int num_threads;
    thread_t *threads;
    process_queue queue;
    int num_proc_queue;
} core_t;

typedef struct {
    int id;
    int num_cores;
    core_t *cores;
} cpu_t;

typedef struct {
    cpu_t *cpus;
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