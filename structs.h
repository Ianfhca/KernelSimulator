#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

pid_t gettid(void);
void *malloc(size_t size);

typedef struct {
    pid_t tid; // process id
    int status; // 0 = ready; 1 = waitting; 2 = blocked;
    double quantum; // process 
} PCB;

struct scheduler {
    struct PCB * runqueue;
};

typedef struct {
    int cycles1;
    int cycles2;
    int count1;
    int count2;
    PCB pcb;
} args_t;

// struct cfs_rq {

// };

// struct rt_rq {

// };

// struct run_queue {
//     unsigned int np_running;
//     struct cfs_rq cfs;
//     struct rt_rq rt;
//     struct PCB *curr;
//     int cpu;
// };