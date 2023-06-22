#include "scheduler.h"

#include "process_queue.h"

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
                        printf("--*EXECUTING %d* -> CPU %d: C%d - T%d\n", pcb_aux.pid, cpu_id, core_id, thread_id);
                        fflush(stdout);
                    } else {
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
                machine->cpus[cpu_id].cores[core_id].threads[thread_id].pcb.status = NULL_S;
                printf("--(ENQUEUED %d) -> CPU %d: C%d - T%d\n", pcb_aux.pid, cpu_id, core_id, thread_id);
                fflush(stdout);
            } else {
                pcb_aux = machine->cpus[cpu_id].cores[core_id].threads[thread_id].pcb;
                machine->cpus[cpu_id].cores[core_id].threads[thread_id].pcb.status = FINISHED;
                machine->cpus[cpu_id].cores[core_id].num_proc_queue--;
                process_map[pcb.pid] = 0; /*Free pid*/
                printf("--FINISHED %d -> CPU %d: C%d - T%d\n", pcb_aux.pid, cpu_id, core_id, thread_id);
                fflush(stdout);
            }
            break;
        case BLOCKED:
            /*Implemention for blocked process*/
            break;
        case FINISHED:

            break;
        default:
            /*NULL_S process*/
            break;
    }
}

void scheduler(machine_t *machine) {
    int cpu_id, core_id, thread_id;
    pcb_t pcb;

    printf("-Scheduling time:\n");
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

    while (1) {
        pulses++;
        if (pulses == args->freq_schl) {
            pulses = 0;
            /*pthread();*/
            scheduler(&args->machine);
        }
        args->done++;
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond2, &mutex);
    }
    pthread_mutex_unlock(&mutex);
}