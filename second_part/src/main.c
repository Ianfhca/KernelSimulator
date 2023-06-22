#include "init.h"
#include "process_generator.h"
#include "scheduler.h"

pthread_mutex_t mutex;
pthread_cond_t cond1;
pthread_cond_t cond2;

int num_cpus;
int num_cores;
int num_threads;

int process_map[MAX_PROCESS];

int exit_process;

void* cpu_clock(void* arguments);

/**
 * This function symulates CPU clock frecuence.
 * Parameters:
 *  - *arguments: arg_t type struct
 * Return:
 */
void* cpu_clock(void* arguments) {
    args_t* args = arguments;
    int cpu_id, core_id, thread_id;
    exit_process = 0;

    while (1) {
        pthread_mutex_lock(&mutex);

        while (args->done < NUM_TIMERS) {
            pthread_cond_wait(&cond1, &mutex); /*Unlocks the mutex, waits for signal and it locks again*/
        }
        for (cpu_id = 0; cpu_id < args->machine.num_cpus; cpu_id++) {
            for (core_id = 0; core_id < args->machine.cpus[cpu_id].num_cores; core_id++) {
                for (thread_id = 0; thread_id < args->machine.cpus[cpu_id].cores[core_id].num_threads; thread_id++) {
                    if (args->machine.cpus[cpu_id].cores[core_id].threads[thread_id].pcb.status == EXECUTING) {
                        args->machine.cpus[cpu_id].cores[core_id].threads[thread_id].pcb.live_time--;
                        args->machine.cpus[cpu_id].cores[core_id].threads[thread_id].pcb.quantum--;
                        if (args->machine.cpus[cpu_id].cores[core_id].threads[thread_id].pcb.live_time <= 0) {
                            args->machine.cpus[cpu_id].cores[core_id].threads[thread_id].pcb.status = FINISHED;
                            if (exit_process == 0) exit_process = 1;
                        }
                    }
                }
            }
        }

        if (exit_process == 1) {
            printf("\033[1;31m");
            printf("*Scheduler interruption to get out finished processes*\n");
            printf("\033[0m");
            fflush(stdout);
            scheduler(&args->machine);
        }

        args->done = 0;
        pthread_cond_broadcast(&cond2);
        pthread_mutex_unlock(&mutex);
    }
}

int main(void) {
    int i;

    args_t args;
    args.freq_pgen[0] = 999;
    args.freq_pgen[1] = 3999;
    args.freq_schl = 5000;
    args.done = 0;
    args.finished = 0;

    srand(time(NULL));

    if (init_config(&args) == 1) return (0);

    pthread_t th[NUM_THREADS];
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond1, NULL);
    pthread_cond_init(&cond2, NULL);
    if (pthread_create(&th[0], NULL, &cpu_clock, &args) != 0) perror("Failed to create th1\n");
    if (pthread_create(&th[1], NULL, &timer1, &args) != 0) perror("Failed to create th2\n");
    if (pthread_create(&th[2], NULL, &timer2, &args) != 0) perror("Failed to create th3\n");

    /*
    sleep(1);
    input = '\0';
    while ((input = getchar()) != '\n');

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_cancel(th[i]);
    }
    */

    for (i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(th[i], NULL) != 0) perror("Failed to join thread\n");
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond1);
    pthread_cond_destroy(&cond2);

    printf("The simulation has finished");
    return 0;
}