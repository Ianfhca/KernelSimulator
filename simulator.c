#include "./functions.h"

long num_processors;

pthread_mutex_t mutex;
pthread_cond_t cond1;
pthread_cond_t cond2;
int done;

void * cpu_clock(void *arg) {
    printf("Im the clock with tid %d\n", gettid());
    while (1) {
        pthread_mutex_lock(&mutex);
        while (done < MAX_TIMERS) {
            pthread_cond_wait(&cond1, &mutex); // Unlocks the mutex, waits for signal and it locks again
        }
        done = 0;
        pthread_cond_broadcast(&cond2);
        pthread_mutex_unlock(&mutex);
    }
}

void * timer1(void *arg) {
    pthread_mutex_lock(&mutex);
    args_t *args = arg;
    printf("I'm the timer 1 with tid %d\n", gettid());
    while (1) {
        done++;
        args->cycles1++;
        if (args->cycles1 == args->count2) {
            //printf("\n%d ha contado %d ciclos\n", gettid(), args->cycles1);
            //fflush(stdout);
            args->cycles1 = 0;
            execute_process(&num_processors);
        }
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond2, &mutex);
    }
}

void * timer2(void *arg) {
    pthread_mutex_lock(&mutex);
    args_t *args = arg;
    printf("I'm the timer 2 with tid %d\n", gettid());
    while (1) {
        done++;
        args->cycles2++;
        if (args->cycles2 == args->count1) {
            // printf("\n%d ha contado %d ciclos\n", gettid(), args->cycles2);
            // fflush(stdout);
            args->cycles2 = 0;
            generate_process();
        }
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond2, &mutex);
    }
}

int main(int argc, char *argv[]) {
    num_processors = sysconf(_SC_NPROCESSORS_ONLN);
    printf("The number of processors is %ld\n", num_processors);

    pthread_t th[NUM_THREADS];
    args_t args;
    args.cycles1 = 0;
    args.cycles2 = 0;
    args.count1 = 10000;
    args.count2 = 50000;

    // pcb_queue = (PCB*) malloc(200 * sizeof(PCB));
    srand(time(NULL));

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond1, NULL);
    pthread_cond_init(&cond2, NULL);
    if (pthread_create(&th[0], NULL, &cpu_clock, NULL) != 0) perror("Failed to create th1\n");
    if (pthread_create(&th[1], NULL, &timer1, &args) != 0) perror("Failed to create th2\n");
    if (pthread_create(&th[2], NULL, &timer2, &args) != 0) perror("Failed to create th3\n");

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(th[i], NULL) != 0) perror("Failed to join thread\n");
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond1);
    pthread_cond_destroy(&cond2);
    return 0;
}

// ------------------------------------------------------ MORRALLA -----------------------------------------------------------------