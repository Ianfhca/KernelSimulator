#ifndef P_GEN
#define P_GEN

#include "globals.h"

void *timer1(void *args);
void generate_process(machine_t *machine, pcb_t *pcb, int num_process);
void create_pcb(pcb_t *pcb, int num_process);

#endif