#ifndef LDR
#define LDR

#include "globals.h"

pcb_t create_pcb(int pid, int frequence);
void generate_process(machine_t *machine, int frequence);
void loader(machine_t *machine);
void *timer1(void *arguments);

#endif