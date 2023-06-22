#ifndef QUEUE
#define QUEUE

#include "globals.h"

#define MAX_PROCESS_QUEUE 64

void inic_queue(process_queue* queue);
int is_empty(process_queue* queue);
void enqueue(process_queue* queue, pcb_t pcb);
pcb_t dequeue(process_queue* queue);
void empty_queue(process_queue* queue);
int queue_size(process_queue* queue);
pcb_t get_first_process(process_queue* queue);
pcb_t get_null_process();

#endif