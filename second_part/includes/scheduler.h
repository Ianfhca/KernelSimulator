#ifndef SCH
#define SCH

#include "globals.h"

void dispatcher(machine_t *machine, pcb_t pcb, int cpu_id, int core_id, int thread_id);
void scheduler(machine_t *machine);
void *scheduler1(void *arguments);
void *timer2(void *arguments);

#endif