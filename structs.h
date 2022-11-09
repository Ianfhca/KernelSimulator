#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

pid_t gettid(void);
void *malloc(size_t size);

struct PCB {
    pid_t tid;
};

struct cfs_rq {

};

struct rt_rq {

};

struct run_queue {
    unsigned int np_running;
    struct cfs_rq cfs;
    struct rt_rq rt;
    struct PCB *curr;
    int cpu;
};

typedef struct {
    int cycles;
    int quantum;
    struct PCB pcb;
} args_t;