#include "./functions.h"

long num_processors;
pthread_t *pth;
unsigned long num_process;

pthread_mutex_t mutex;
pthread_cond_t cond1;
pthread_cond_t cond2;

node *current = NULL;
node *newprocess = NULL;

// node *first = NULL;
// node *last = NULL;

/**
 * Funtion that symulates CPU's clock frecuency
 * 
*/
void * cpu_clock(void *arg) {
    args_t *args = arg;
    printf("Im the clock with tid %d\n", gettid());
    while (1) {
        pthread_mutex_lock(&mutex);
        while (args->done < MAX_TIMERS) {
            pthread_cond_wait(&cond1, &mutex); // Unlocks the mutex, waits for signal and it locks again
        }
        args->done = 0;
        pthread_cond_broadcast(&cond2);
        pthread_mutex_unlock(&mutex);
    }
}

/**
 * Function that generates processes with random properties
 * void *arg is an struct
*/
void * timer1(void *arg) {
    pthread_mutex_lock(&mutex);
    args_t *args = arg;
    printf("I'm the timer 2 with tid %d\n", gettid());
    while (1) {
        args->done++;
        args->cycles2++;
        if (args->cycles2 == args->count1) {
            args->cycles2 = 0;
            // while (1) {
                if (num_process < MAX_PROCESS) {
                    newprocess = generate_process(num_process);
                    add_process(newprocess);
                    num_process++;
                    // break;
                }
            // }
        }
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond2, &mutex);
    }
}

/**
 * Function 
 * 
*/
void * timer2(void *arg) {
    pthread_mutex_lock(&mutex);
    args_t *args = arg;
    printf("I'm the timer 1 with tid %d\n", gettid());
    while (1) {
        args->done++;
        args->cycles1++;
        if (args->cycles1 == args->count2) {
            args->cycles1 = 0;
            // while (1) {
                if (num_processors >= 0) { // revisar que sea > o >=
                    current = get_first();
                    // printf("*LIVE_TIME - PRE %d\n", current->this.live_time);
                    if (pthread_create(&pth[num_processors-1], NULL, &execute_process, current) != 0)
                        perror("Failed to create thread\n");
                    num_processors--;
                    if (pthread_join(pth[num_processors], NULL) != 0) // Falta añadir el contador de ciclos
                        perror("Failed to join thread\n");
                    // printf("*LIVE_TIME - POST %d\n", current->this.live_time);
                    if (current->this.live_time > 0) {
                        // Volver a meterlo en la cola
                        add_process(current);
                    } else {
                        free(current);
                        num_process++;
                    }
                    num_processors++;
                    // break;
                }
            // }
            // execute_process(&num_processors);
        }
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond2, &mutex);
    }
}

int main(int argc, char *argv[]) {
    num_processors = sysconf(_SC_NPROCESSORS_ONLN);
    pth = (pthread_t *)malloc(num_processors * sizeof(pthread_t));
    printf("The number of processors is %ld\n", num_processors);
    num_process = 0;

    pthread_t th[NUM_THREADS];
    args_t args;
    args.cycles1 = 0;
    args.cycles2 = 0;
    args.count1 = 9999;
    args.count2 = 40000;
    args.done = 0;

    current = (node *)malloc(sizeof(node));
    newprocess = (node *)malloc(sizeof(node));

    srand(time(NULL));

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond1, NULL);
    pthread_cond_init(&cond2, NULL);
    if (pthread_create(&th[0], NULL, &cpu_clock, &args) != 0) perror("Failed to create th1\n");
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
/*
#include "./functions.h"

long num_processors;

pthread_mutex_t mutex;
pthread_cond_t cond1;
pthread_cond_t cond2;
int *cont;


void * cpu_clock(void *arg) {
    args_t *args = arg;
    printf("Im the clock with tid %d\n", gettid());
    while (1) {
        pthread_mutex_lock(&mutex);
        while ((*args->done) < MAX_TIMERS) {
            pthread_cond_wait(&cond1, &mutex); // Unlocks the mutex, waits for signal and it locks again
        }
        (*args->done) = 0;
        pthread_cond_broadcast(&cond2);
        pthread_mutex_unlock(&mutex);
    }
}


void * timer1(void *arg) {
    pthread_mutex_lock(&mutex);
    args_t *args = arg;
    printf("I'm the timer 1 with tid %d\n", gettid());
    while (1) {
        (*args->done)++;
        args->cycles++;
        if (args->cycles >= args->count) {
            //printf("\n%d ha contado %d ciclos\n", gettid(), args->cycles1);
            //fflush(stdout);
            args->cycles = 0;
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
        (*args->done)++;
        args->cycles++;
        if (args->cycles >= args->count) {
            // printf("\n%d ha contado %d ciclos\n", gettid(), args->cycles2);
            // fflush(stdout);
            args->cycles = 0;
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
    args_t args1;
    args_t args2;
    cont = (int *) malloc(sizeof(int));
    (*cont) = 0;
    args.done = cont;
    args1.cycles = 0;
    args2.cycles = 0;
    args1.count = 9999;
    args2.count = 40000;
    args1.done = cont;
    args2.done = cont;
    printf("args %d\n", *args.done);
    printf("args1 %d\n", *args1.done);
    printf("args2 %d\n", *args2.done);

    srand(time(NULL));

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond1, NULL);
    pthread_cond_init(&cond2, NULL);
    if (pthread_create(&th[0], NULL, &cpu_clock, &args) != 0) perror("Failed to create th1\n");
    if (pthread_create(&th[1], NULL, &timer1, &args1) != 0) perror("Failed to create th2\n");
    if (pthread_create(&th[2], NULL, &timer2, &args2) != 0) perror("Failed to create th3\n");

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(th[i], NULL) != 0) perror("Failed to join thread\n");
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond1);
    pthread_cond_destroy(&cond2);
    return 0;
}
*/