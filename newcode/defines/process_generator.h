#ifndef P_GEN
#define P_GEN

#include "globals.h"
#include "queue.h"

void *timer1(void *args);
void generate_process(pcb_t *pcb, int num_process);
void create_pcb(pcb_t *pcb, int num_process);

#endif