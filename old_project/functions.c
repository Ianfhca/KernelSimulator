#include "./functions.h"

//long num_processors;
// int num_process = 0;

node *first = NULL;
node *last = NULL;

node *generate_process(unsigned long num_process) {
    // if (num_process < MAX_PROCESS) {
        node *newnode = malloc(sizeof(node));
        newnode->this.pid = num_process;
        newnode->this.status = 0;
        newnode->this.live_time = (rand() % 80) + 20; // random value between 20 and 100
        newnode->this.quantum = 50;
        newnode->next = NULL;
        // add_process(newnode);
        num_process++;
        printf("New process added (%d)\n", newnode->this.pid);
        fflush(stdout);
        return newnode;
    // }
    // return NULL;
}

void *execute_process(void *new_process) {
    // if ((*num_processors) > 0) {
        node *process = new_process;
        if (process != NULL) {
            // PCB pcb = process->this;
            // node *current = process;
            if (first == last) {
                first = NULL;
                last = NULL;
            } else {
                first = first->next;
            }
            process->this.live_time = (process->this.live_time - process->this.quantum);
            printf(" - Executing process %d...\n", process->this.pid);
            fflush(stdout);            
        }
        // mirar si el tiempo de vida del proceso acabo y si no volver a meterlo en la cola
        // Cuando esto termine hay que incrementar el num_processors
    // }
}

node *get_first() {
    return first;
}

node *get_last() {
    return last;
}

void add_process (node *newnode) {
    if (first == NULL) {
        first = newnode;
        last = newnode;
    } else {
        last->next = newnode;
        last = newnode;
    }
}