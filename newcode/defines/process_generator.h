#ifndef P_GEN
#define P_GEN

#include "globals.h"

void *timer1(void *args);
void generate_process(machine_t *machine, int pid);
pcb_t create_pcb(int pid);

#endif