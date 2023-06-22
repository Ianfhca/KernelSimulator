#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

#define MAX_TIMERS 2
#define NUM_THREADS MAX_TIMERS+1
#define MAX_PROCESS 10

pid_t gettid(void);
void *malloc(size_t size);

typedef struct {
    int pid; // process id
    int status; // 0 = ready; 1 = waitting; 2 = blocked;
    signed int live_time; // process live time
    int quantum; // process time spent in CPU
    int thread;
} PCB;

struct node_t {
    PCB this;
    struct node_t *next;
};
typedef struct node_t node;

// typedef struct {
//     node *first;
//     node *last;
// } QUEUE;

typedef struct {
    int cycles1;
    int cycles2;
    int count1;
    int count2;
    int done;
    int finished;
    int num_timers;
    // node queue;
} args_t;