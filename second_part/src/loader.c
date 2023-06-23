#include "loader.h"

#include "process_queue.h"

pcb_t create_pcb(int pid, int frequence) {
    pcb_t pcb;
    pcb.pid = pid;
    pcb.status = READY;
    pcb.load_quantum = (rand() % (frequence * 4)) + (frequence / 3); /*random value between (frequence / 3) and (frequence * 6)*/
    pcb.quantum = pcb.load_quantum;                                  /*random value between (frequence / 3) and (frequence * 6)*/
    pcb.live_time = (rand() % (pcb.quantum * 4)) + (pcb.quantum);    /*random value between (pcb.quantum) and (pcb.quantum * 5)*/
    pcb.priority = (rand() % 100);                                   /*random value between 0 and 100*/
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
            if (pcb.load_quantum > frequence)
                phases = (pcb.load_quantum / frequence) + 1;
            else
                phases = (frequence / pcb.load_quantum);
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

void loader(machine_t *machine) {
    char folder[] = "programs/";
    char name[] = "prog";
    static int num[3] = {0, 0, 0};
    char ext[] = ".elf";
    char file_name[12];
    FILE *file;

    __uint32_t text_addr, data_addr;
    int exit_inst = 0;

    if (num[1] <= 5) {
        sprintf(file_name, "%s%d%d%d%s", name, num[0], num[1], num[2], ext);

        if ((file = fopen(strcat(folder, file_name), "r")) == NULL) printf("File %s can not be opened\n", file_name);

        char line[20];
        __uint32_t value;

        while (fgets(line, sizeof(line), file) != NULL) {
            line[strlen(line) - 1] = '\0';
            if (exit_inst == 0) {
                switch (line[0]) {
                    case '.':
                        sscanf(&line[6], "%x", &value);
                        if (line[1] == 't') {
                            text_addr = value;
                            printf("%s    .text                  @ 0x%06X\n", file_name, text_addr);
                            printf("---------------------------------------\n");
                        } else {
                            data_addr = value;
                        }
                        break;
                    case '0':
                        sscanf(&line[2], "%x", &value);
                        printf("0x%06X: [%s]  ld    r%c, 0x%06X\n", text_addr, line, line[1], value);
                        text_addr += WORD;
                        break;
                    case '1':
                        sscanf(&line[2], "%x", &value);
                        printf("0x%06X: [%s]  st    r%c, 0x%06X\n", text_addr, line, line[1], value);
                        text_addr += WORD;
                        break;
                    case '2':
                        printf("0x%06X: [%s]  add   r%c, r%c, r%c\n", text_addr, line, line[1], line[2], line[3]);
                        text_addr += WORD;
                        break;

                    case 'F':
                        exit_inst = 1;
                        printf("0x%06X: [%s]  exit\n", text_addr, line);
                        break;
                    default:
                        printf("Error: This instruction is not used int this program.\n");
                        break;
                }
            } else {
                /*printf(".data                  @ 0x%06X\n", data_addr);
                printf("-------------------------------\n");*/
                sscanf(&line[0], "%x", &value);
                printf("0x%06X: [%s]  %d\n", data_addr, line, value);
                data_addr += WORD;
            }
        }
        num[2]++;
        if (num[2] > 9) {
            num[2] = 0;
            num[1]++;
            if (num[1] > 9) {
                num[1] = 0;
                num[0]++;
            }
        }

        fclose(file);
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
            loader(&args->machine);
        }
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond2, &mutex);
    }
    pthread_mutex_unlock(&mutex);
}