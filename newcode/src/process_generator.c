#include "process_generator.h"

#include "process_queue.h"

void create_pcb(pcb_t *pcb, int pid) {
    pcb->pid = pid;
    pcb->state = READY;
    pcb->live_time = (rand() % 80) + 20; /*random value between 20 and 100*/
    pcb->priority = (rand() % 100);      /*random value between 0 and 100*/
}

void generate_process(machine_t *machine, pcb_t *pcb, int pid) {
    int i, j;
    int size, min = INT_MAX, cpu_id = -1, core_id = -1;

    for (i = 0; i < machine->num_cpus; i++) {
        for (j = 0; j < machine->cpus[i].num_cores; j++) {
            size = queue_size(&machine->cpus[i].cores[j].queue);
            if (size < MAX_PROCESS_QUEUE - 1 && size < min) {
                min = size;
                cpu_id = i;
                core_id = j;
            }
        }
    }
    if (core_id != -1) {
        create_pcb(pcb, pid);
        enqueue(&machine->cpus[cpu_id].cores[core_id].queue, pcb);
    } else {
        printf("Warning: All core queues are full\n");
        fflush(stdout);
    }
}

/**
 * This function counts clock pulses and generates a processe when pulses reach established frecuence.
 * Parameters:
 *  - *arg: arg_t type struct
 * Return:
 */
void *timer1(void *arguments) {
    pthread_mutex_lock(&mutex);
    args_t *args = arguments;
    pcb_t pcb;
    int pulses = 0, pid = 0;
    int freq_pgen = args->freq_pgen[0] + rand() % (args->freq_pgen[1] - args->freq_pgen[0] + 1);

    while (1) {
        args->done++;
        pulses++;
        if (pulses == freq_pgen) {
            pulses = 0;
            while (process_map[pid] != 0 && pid < MAX_PROCESS) {
                pid++;
            }
            if (pid < MAX_PROCESS) {
                process_map[pid] = 1;
                generate_process(&args->machine, &pcb, pid);
                freq_pgen = args->freq_pgen[0] + rand() % (args->freq_pgen[1] - args->freq_pgen[0] + 1);
                printf("Generating process: %d\n", pid);
                fflush(stdout);
            } else {
                printf("Maximum number of process\n");
                fflush(stdout);
            }
        }
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond2, &mutex);
    }
    pthread_mutex_unlock(&mutex);
}