#include "globals.h"
#include "queue.h"

void inic_queue(process_queue* queue) {
    queue->first = NULL;
    queue->last = NULL;
}

int is_empty(process_queue* queue) {
    return queue->first == NULL;
}

void enqueue(process_queue* queue, pcb_t pcb) {
    node_t* node = (node_t*)malloc(sizeof(node_t));
    node->pcb = pcb;
    node->next = NULL;

    if (is_empty(queue)) {
        queue->first = node;
        queue->last = node;
    } else {
        queue->last->next = node;
        queue->last = node;
    }
}

pcb_t dequeue(process_queue* queue) {
    if (is_empty(queue)) {
        pcb_t null_process;
        null_process.pid = -1;
        null_process.state = 0;
        return null_process;
    }

    node_t* node = queue->first;
    pcb_t pcb = node->pcb;
    queue->first = queue->first->next;

    if (queue->first == NULL) {
        queue->last = NULL;
    }

    free(node);
    return pcb;
}

void empty_queue(process_queue* cola) {
    while (!is_empty(cola)) {
        dequeue(cola);
    }
}