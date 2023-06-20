#include "../defines/process_generator.h"

void create_pcb(pcb_t *pcb, int pid) {
    pcb->pid = pid;
    pcb->state = 0;
    pcb->live_time = (rand() % 80) + 20; /*random value between 20 and 100*/
}

void generate_process(pcb_t *pcb, int pid) {
    create_pcb(pcb, pid);
    enqueue();
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
                generate_process(&pcb, pid);
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