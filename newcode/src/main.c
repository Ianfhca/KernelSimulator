#include "../defines/globals.h"
#include "../defines/process_generator.h"
#include "../defines/scheduler.h"

pthread_mutex_t mutex;
pthread_cond_t cond1;
pthread_cond_t cond2;

/**
 * Funtion that symulates CPU's clock frecuency
 *
 */
void *cpu_clock(void *arg) {
    args_t *args = arg;
    printf("Im the clock with tid %d\n", gettid());
    while (1) {
        pthread_mutex_lock(&mutex);
        while (args->done < NUM_TIMERS) {
            pthread_cond_wait(&cond1, &mutex);  // Unlocks the mutex, waits for signal and it locks again
        }
        args->done = 0;
        pthread_cond_broadcast(&cond2);
        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        printf("Error: No se permiten argumentos para este programa.\n");
        return (1);
    }

    char input = ' ';
    machine_t machine;

    printf("+-------------- KERNEL SIMULATOR --------------+\n");
    printf("| C = Configure machine                        |\n");
    printf("| S = Start simulation                         |\n");
    printf("+----------------------------------------------+\n");

    while (input != 's' && input != 'S') {
        scanf(" %c", &input);

        switch (input) {
            case 'c': case 'C':
                printf("Set the number of CPUs: ");
                scanf("%d", &machine.num_CPUs);
                printf("Set the number of cores per CPU: ");
                scanf("%d", &machine.num_cores);
                printf("Set the number of threads per core: ");
                scanf("%d", &machine.num_threads);
                printf("Machine has been configured successfully\n");
                break;
            case 's': case 'S':
                printf("Starting simulation\n");
                break;

            default:
                printf("(%c) is not a valid code. Please try again.\n", input);
                break;
        }
    }
    

    pthread_t th[NUM_THREADS];

    args_t args;
    args.freq_pgen = 9999;
    args.freq_schl = 40000;
    args.done = 0;
    args.finished = 0;

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

    printf("The simulation is has finished");
    return 0;
}