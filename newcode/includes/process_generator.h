#ifndef P_GEN
#define P_GEN

#include "globals.h"

pcb_t create_pcb(int pid, int frequence);
void generate_process(machine_t *machine, int frequence);
void *timer1(void *arguments);

#endif