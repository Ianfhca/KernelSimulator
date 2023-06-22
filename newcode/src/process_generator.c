#include "../includes/process_generator.h"
#include "../includes/process_queue.h"
#include <math.h>

pcb_t create_pcb(int pid, int frequence) {
    pcb_t pcb;
    pcb.pid = pid;
    pcb.status = READY;
    pcb.load_quantum = (rand() % (frequence * 4)) + (frequence / 3);  /*random value between (frequence / 3) and (frequence * 6)*/
    pcb.quantum = pcb.load_quantum;                                   /*random value between (frequence / 3) and (frequence * 6)*/
    pcb.live_time = (rand() % (pcb.quantum * 4)) + (pcb.quantum); /*random value between (pcb.quantum) and (pcb.quantum * 5)*/
    pcb.priority = (rand() % 100);                                    /*random value between 0 and 100*/
    return pcb;
}

void generate_process(machine_t *machine, int frequence) {
    int i, j, size, phases;
    int pid = 0, min = INT_MAX, cpu_id = -1, core_id = -1;
    pcb_t pcb;

    /*Select free pid*/
    while (process_map[pid] != 0 && pid < MAX_PROCESS) {
        pid++;
    }
    if (pid < MAX_PROCESS) {
        process_map[pid] = 1;

        /*Enqueue process in the emptiest core queue*/
        for (i = 0; i < machine->num_cpus; i++) {
            for (j = 0; j < machine->cpus[i].num_cores; j++) {
                /*size = queue_size(&machine->cpus[i].cores[j].queue);*/
                size = machine->cpus[i].cores[j].num_proc_queue;
                if (size < MAX_PROCESS_QUEUE && size < min) {
                    min = size;
                    cpu_id = i;
                    core_id = j;
                }
            }
        }
        if (core_id != -1) {
            pcb = create_pcb(pid, frequence);
            enqueue(&machine->cpus[cpu_id].cores[core_id].queue, pcb);
            machine->cpus[cpu_id].cores[core_id].num_proc_queue++;
            if (pcb.load_quantum > frequence) phases = (pcb.load_quantum / frequence) + 1;
            else phases = (frequence / pcb.load_quantum);
            printf("\033[1;37m");
            printf("Process Generated %d: [CPU %d-> CORE %d] | TTL %d, Quantum %d (%d CPU Phases)\n", pid, cpu_id, core_id, pcb.live_time, pcb.load_quantum, phases);
            printf("\033[0m");
            fflush(stdout);
        } else {
            printf("\033[1;35m");
            printf("Warning: All core queues are full\n");
            printf("\033[0m");
            fflush(stdout);
        }
    } else {
        printf("\033[1;31m");
        printf("Maximum number of process\n");
        printf("\033[0m");
        fflush(stdout);
    }
}

/**
 * This function counts clock pulses and generates a processe when pulses reach established frecuence.
 * Parameters:
 *  - *arguments: arg_t type struct
 * Return:
 */
void *timer1(void *arguments) {
    pthread_mutex_lock(&mutex);
    args_t *args = arguments;
    int pulses = 0;
    int freq_pgen = args->freq_pgen[0] + rand() % (args->freq_pgen[1] - args->freq_pgen[0] + 1);

    while (1) {
        args->done++;
        pulses++;
        if (pulses == freq_pgen) {
            pulses = 0;
            freq_pgen = args->freq_pgen[0] + rand() % (args->freq_pgen[1] - args->freq_pgen[0] + 1);
            generate_process(&args->machine, args->freq_schl);
        }
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond2, &mutex);
    }
    pthread_mutex_unlock(&mutex);
}