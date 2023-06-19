#include "../defines/globals.h"
#include "../defines/process_generator.h"
#include "../defines/scheduler.h"

pthread_mutex_t mutex;
pthread_cond_t cond1;
pthread_cond_t cond2;
int num_cpus;
int num_cores;
int num_threads;

/**
 * Funtion that symulates CPU's clock frecuency
 *
 */
void* cpu_clock(void* arg) {
    args_t* args = arg;
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    // printf("Im the clock with tid %d\n", gettid());
    while (1) {
        pthread_mutex_lock(&mutex);
        while (args->done < NUM_TIMERS) {
            pthread_cond_wait(&cond1, &mutex);  // Unlocks the mutex, waits for signal and it locks again
        }
        args->done = 0;
        pthread_testcancel();
        pthread_cond_broadcast(&cond2);
        pthread_mutex_unlock(&mutex);
    }
}

int main(void) {
    char input = '\0';

    num_cpus = 2;
    num_cores = 2;
    num_threads = 8;
    machine_t machine;

    args_t args;
    args.freq_pgen[0] = 1000;
    args.freq_pgen[1] = 4000;
    args.freq_schl = 5000;
    args.done = 0;
    args.finished = 0;

    printf("+-------------- KERNEL SIMULATOR --------------+\n");
    printf("| C = Configure simulator                      |\n");
    printf("| D = Launch default simulation                |\n");
    printf("| R = Run simulation                           |\n");
    printf("| S = Stop simulation                          |\n");
    printf("| E = Exit                                     |\n");
    printf("+----------------------------------------------+\n");

    while (input != 'r' && input != 'R') {
        scanf(" %c", &input);

        switch (input) {
            case 'c':
            case 'C':
                printf("Set the number of CPUs: ");
                scanf("%d", &num_cpus);
                printf("Set the number of cores per CPU: ");
                scanf("%d", &num_cores);
                printf("Set the number of threads per core: ");
                scanf("%d", &num_threads);
                printf("Set the first frequence range of process generator: ");
                scanf("%d", &args.freq_pgen[0]);
                printf("Set the second frequence range of process generator: ");
                scanf("%d", &args.freq_pgen[1]);
                printf("Set the frequence of scheduler: ");
                scanf("%d", &args.freq_schl);
                printf("Machine has been configured successfully\n");
                break;
            case 'd':
            case 'D':
                num_cpus = 2;
                num_cores = 2;
                num_threads = 8;
                args.freq_pgen[0] = 1000;
                args.freq_pgen[1] = 1000;
                args.freq_schl = 5000;
                printf("Default values has been set:\n");
                printf("CPUs: %d, Cores: %d, Threads: %d\n", num_cpus, num_cores, num_threads);
                printf("Process generator frequence: %d - %d \nScheduler/Dispatcher frequence %d\n", args.freq_pgen[0], args.freq_pgen[1], args.freq_schl);
                input = 'r';
                break;
            case 'r':
            case 'R':
                printf("Running kernel simulator\n");
                break;
            case 's':
            case 'S':
                printf("First try to run the simulator\n");
                break;
            case 'e':
            case 'E':
                printf("Exit program\n");
                return (0);
                break;
            default:
                printf("(%c) is not a valid code. Please try again.\n", input);
                break;
        }
    }

    machine.num_cpus = num_cpus;
    machine.cpus = (CPU*)malloc(machine.num_cpus * sizeof(CPU));  // CPUs
    printf("\n");
    for (int cpu_id = 0; cpu_id < machine.num_cpus; cpu_id++) {
        CPU* cpu = &(machine.cpus[cpu_id]);
        cpu->id = cpu_id;
        cpu->num_cores = num_cores;
        printf("CPU %d:\n", cpu->id);

        cpu->cores = (Core*)malloc(cpu->num_cores * sizeof(Core));  // Cores per CPU

        for (int core_id = 0; core_id < cpu->num_cores; core_id++) {
            Core* core = &(cpu->cores[core_id]);
            core->id = core_id;
            core->num_threads = num_threads;
            printf(" -C%d: ", core->id);

            core->threads = (Thread*)malloc(core->num_threads * sizeof(Thread));  // Threads per core

            for (int thread_id = 0; thread_id < core->num_threads; thread_id++) {
                Thread* thread = &(core->threads[thread_id]);
                thread->cpu_id = cpu_id;
                thread->core_id = core_id;
                thread->id = thread_id;
                printf("T%d ", thread->id);
                // pthread_create(&(thread->thread_id), NULL, thread_function, (void*)thread);
            }
            printf("\n");
        }
        printf("\n");
    }

    pthread_t th[NUM_THREADS];
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond1, NULL);
    pthread_cond_init(&cond2, NULL);
    if (pthread_create(&th[0], NULL, &cpu_clock, &args) != 0) perror("Failed to create th1\n");
    if (pthread_create(&th[1], NULL, &timer1, &args) != 0) perror("Failed to create th2\n");
    if (pthread_create(&th[2], NULL, &timer2, &args) != 0) perror("Failed to create th3\n");

    // sleep(1);
    // input = '\0';
    // while ((input = getchar()) != '\n');

    // for (int i = 0; i < NUM_THREADS; i++) {
    //     pthread_cancel(th[i]);
    // }

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(th[i], NULL) != 0) perror("Failed to join thread\n");
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond1);
    pthread_cond_destroy(&cond2);

    printf("The simulation is has finished");
    return 0;
}