#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

#define MAX_TIMERS 2
#define NUM_THREADS MAX_TIMERS+1
#define MAX_PROCESS 100

pid_t gettid(void);
void *malloc(size_t size);

typedef struct {
    pid_t tid; // process id
    int status; // 0 = ready; 1 = waitting; 2 = blocked;
    int live_time; // 
    double quantum; // process time spent in CPU
} PCB;

struct node_t {
    PCB this;
    struct node_t *next;
};
typedef struct node_t node;

typedef struct {
    int cycles1;
    int cycles2;
    int count1;
    int count2;
    PCB pcb;
} args_t;