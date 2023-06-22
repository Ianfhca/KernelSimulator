#include "scheduler.h"

#include "process_queue.h"

pthread_mutex_t s_mutex;
pthread_cond_t s_cond;
int ready = 0;

void dispatcher(machine_t *machine, pcb_t pcb, int cpu_id, int core_id, int thread_id) {
    pcb_t pcb_aux;

    switch (pcb.status) {
        case READY:
            for (thread_id = 0; thread_id < machine->cpus[cpu_id].cores[core_id].num_threads; thread_id++) {
                if (machine->cpus[cpu_id].cores[core_id].threads[thread_id].pcb.status == NULL_S || machine->cpus[cpu_id].cores[core_id].threads[thread_id].pcb.status == FINISHED) {
                    pcb_aux = dequeue(&machine->cpus[cpu_id].cores[core_id].queue);
                    if (pcb_aux.status == READY) {
                        pcb_aux.status = EXECUTING;
                        machine->cpus[cpu_id].cores[core_id].threads[thread_id].pcb = pcb_aux;
                        printf(":\033[1;34m");
                        printf("  [CPU %d-> CORE %d-> THREAD %d] ->  Executing process %d   ", cpu_id, core_id, thread_id, pcb_aux.pid);
                        printf("\033[0m :\n");
                        fflush(stdout);
                    } else {
                        machine->cpus[cpu_id].cores[core_id].threads[thread_id].pcb = get_null_process();
                        break;
                    }
                }
            }
            break;
        case EXECUTING:
            if (pcb.live_time > 0) {
                pcb_aux = machine->cpus[cpu_id].cores[core_id].threads[thread_id].pcb;
                pcb_aux.quantum = pcb_aux.load_quantum;
                pcb_aux.status = READY;
                enqueue(&machine->cpus[cpu_id].cores[core_id].queue, pcb_aux);
                machine->cpus[cpu_id].cores[core_id].threads[thread_id].pcb = get_null_process();
                printf(":\033[1;32m");
                printf("  [CPU %d-> CORE %d-> THREAD %d] ->  Enqueued process %d    ", cpu_id, core_id, thread_id, pcb_aux.pid);
                printf("\033[0m :\n");
                fflush(stdout);
            }
            break;
        case BLOCKED:
            /*Implemention for blocked process*/
            break;
        case FINISHED:
            printf(":\033[1;31m");
            printf("  [CPU %d-> CORE %d-> THREAD %d] ->  Finished process %d    ", cpu_id, core_id, thread_id, pcb.pid);
            printf("\033[0m :\n");
            fflush(stdout);
            pcb_aux = dequeue(&machine->cpus[cpu_id].cores[core_id].queue);
            if (pcb_aux.status == READY) {
                pcb_aux.status = EXECUTING;
                machine->cpus[cpu_id].cores[core_id].threads[thread_id].pcb = pcb_aux;
                printf(":\033[1;35m");
                printf("  [CPU %d-> CORE %d-> THREAD %d] ->  Executing process %d   ", cpu_id, core_id, thread_id, pcb_aux.pid);
                printf("\033[0m :\n");
                fflush(stdout);
            } else {
                pcb_aux = get_null_process();
                machine->cpus[cpu_id].cores[core_id].threads[thread_id].pcb = pcb_aux;
                printf(":\033[1;35m");
                printf("  [CPU %d-> CORE %d-> THREAD %d] ->  Executing NULL process ", cpu_id, core_id, thread_id);
                printf("\033[0m :\n");
                fflush(stdout);
            }
            machine->cpus[cpu_id].cores[core_id].num_proc_queue--;
            process_map[pcb.pid] = 0; /*Free pid*/
            break;
        default:

            /*NULL_S process*/
            break;
    }
}

void scheduler(machine_t *machine) {
    int cpu_id, core_id, thread_id;
    pcb_t pcb;

    printf("+----------------------------------------------------------+\n");
    printf("|\033[1;37m                         SCHEDULER                       \033[0m |\n");
    printf("+----------------------------------------------------------+\n");
    fflush(stdout);

    for (cpu_id = 0; cpu_id < machine->num_cpus; cpu_id++) {
        for (core_id = 0; core_id < machine->cpus[cpu_id].num_cores; core_id++) {
            for (thread_id = 0; thread_id < machine->cpus[cpu_id].cores[core_id].num_threads; thread_id++) {
                pcb = machine->cpus[cpu_id].cores[core_id].threads[thread_id].pcb;
                if ((pcb.quantum <= 0 || pcb.live_time <= 0) && pcb.status != NULL_S) {
                    dispatcher(machine, pcb, cpu_id, core_id, thread_id); /*Remove process from CPU*/
                }
            }
            pcb = get_first_process(&machine->cpus[cpu_id].cores[core_id].queue);
            if (pcb.status != NULL_S) dispatcher(machine, pcb, cpu_id, core_id, -1); /*Introduce queue processes to CPU*/
        }
    }
    printf("+----------------------------------------------------------+\n\n");
    fflush(stdout);
}

void *scheduler1(void *arguments) {
    pthread_mutex_lock(&s_mutex);

    machine_t *machine = arguments;
    int cpu_id, core_id, thread_id;
    pcb_t pcb;

    printf("+---------------------------------------------------------+\n");
    printf("|\033[1;37m                        SCHEDULER                       \033[0m |\n");
    printf("+---------------------------------------------------------+\n");
    fflush(stdout);

    for (cpu_id = 0; cpu_id < machine->num_cpus; cpu_id++) {
        for (core_id = 0; core_id < machine->cpus[cpu_id].num_cores; core_id++) {
            for (thread_id = 0; thread_id < machine->cpus[cpu_id].cores[core_id].num_threads; thread_id++) {
                pcb = machine->cpus[cpu_id].cores[core_id].threads[thread_id].pcb;
                if (pcb.quantum <= 0) {
                    dispatcher(machine, pcb, cpu_id, core_id, thread_id); /*Remove process from CPU*/
                }
            }
            pcb = get_first_process(&machine->cpus[cpu_id].cores[core_id].queue);
            dispatcher(machine, pcb, cpu_id, core_id, -1); /*Introduce queue processes to CPU*/
        }
    }
    printf("+---------------------------------------------------------+\n\n");
    fflush(stdout);

    ready = 1;
    printf("Esto va primero\n");
    pthread_cond_signal(&s_cond);
    pthread_mutex_unlock(&s_mutex);
    pthread_exit(NULL);
}

/**
 * This function counts clock pulses and call the scheduler when pulses reach established frecuence.
 * Parameters:
 *  - *arguments: arg_t type struct
 * Return:
 */
void *timer2(void *arguments) {
    pthread_mutex_lock(&mutex);
    args_t *args = arguments;
    int pulses = 0;
    /*pthread_t sch;*/

    while (1) {
        pulses++;
        if (exit_process == 1) {
            pulses = 0;
            exit_process = 0;
        }
        if (pulses == args->freq_schl) {
            pulses = 0;
            /*
            pthread_mutex_init(&s_mutex, NULL);
            pthread_cond_init(&s_cond, NULL);
            if (pthread_create(&sch, NULL, &scheduler1, &args->machine) != 0) perror("Failed to create sch pthread\n");

            pthread_mutex_lock(&s_mutex);
            while (!ready) {
                pthread_cond_wait(&s_cond, &s_mutex);
            }

            printf("Hilo: La variable de condición está lista.\n");

            pthread_mutex_unlock(&s_mutex);

            pthread_join(sch, NULL);

            pthread_mutex_destroy(&s_mutex);
            pthread_cond_destroy(&s_cond);
            */

            scheduler(&args->machine);
        }
        args->done++;
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond2, &mutex);
    }
    pthread_mutex_unlock(&mutex);
}