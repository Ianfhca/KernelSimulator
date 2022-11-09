#include "structs.h" // structs args_t & PCB

#define MAX_TIMERS 2
#define NUM_THREADS MAX_TIMERS+1

long num_processors;

pthread_mutex_t mutex;
pthread_cond_t cond1;
pthread_cond_t cond2;
int done;

int jobQueue;
int readyQueue;
int waitQueue;

 void* cpu_clock(void *arg) {
    printf("Im the clock\n");
    while (1) {
        pthread_mutex_lock(&mutex);
        while (done < MAX_TIMERS) {
            pthread_cond_wait(&cond1, &mutex); //unlocks the mutex, wait for signal and locks again
        }
        done = 0;
        pthread_cond_broadcast(&cond2);
        pthread_mutex_unlock(&mutex);
    }
 }

 void* timer(void *arg) {
    pthread_mutex_lock(&mutex);
    printf("Im the timer\n");
    args_t *args = arg;
    args->pcb.tid = gettid();
    printf("Im tid %d\n", args->pcb.tid);
    while (1) {
        done++;
        args->cycles++;
        if (args->cycles == args->quantum) {
            printf("\nSe han contado %d ciclos\n", args->cycles);
            args->cycles = 0;
        }
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond2, &mutex);
    }
 }

int main(int argc, char * argv[]) {

    num_processors = sysconf(_SC_NPROCESSORS_ONLN);
    printf("The number of processors is %ld\n", num_processors);

    pthread_t th[NUM_THREADS];
    args_t args;
    args.cycles = 0;
    args.quantum = 20000;

    //struct PCB pcb;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond1, NULL);
    pthread_cond_init(&cond2, NULL);
    if (pthread_create(&th[0], NULL, cpu_clock, NULL) != 0) perror("Failed to create th1\n");
    if (pthread_create(&th[1], NULL, timer, &args) != 0) perror("Failed to create th2\n");
    if (pthread_create(&th[2], NULL, &timer, &args) != 0) perror("Failed to create th2\n");
    
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(th[i], NULL) != 0) perror("Failed to join th1\n");
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond1);
    pthread_cond_destroy(&cond2);
    return 0;

}