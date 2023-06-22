#ifndef SCHL
#define SCHL

#include "globals.h"

void dispatcher(machine_t *machine, pcb_t pcb, int cpu_id, int core_id, int thread_id);
void scheduler(machine_t *machine);
void *timer2(void *args);

#endif