#ifndef INIT
#define INIT

#include "globals.h"

void init_processmap();
void init_mem_layout(args_t *args);
pcb_t init_pcb();
void init_machine(args_t *args);
int init_config(args_t *args);

#endif