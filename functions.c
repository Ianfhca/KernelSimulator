#include "./functions.h"

//long num_processors;
int num_process = 0;

// PCB * pcb_queue;
pthread_t pth[12];

node *first = NULL;
node *last = NULL;

node * generate_process() {
    if (num_process < MAX_PROCESS) {
        node *newnode = malloc(sizeof(node));
        newnode->this.pid = num_process;
        newnode->this.status = 0;
        newnode->this.live_time = (rand() % 80) + 20; // random value between 20 and 100
        newnode->this.quantum = 50;
        newnode->next = NULL;
        add_process(newnode);
        // if (first == NULL) {
        //     first = newnode;
        //     last = newnode;
        // } else {
        //     last->next = newnode;
        //     last = newnode;
        // }
        num_process++;
        // printf("\nSe ha generado el proceso con id %d\n", pcb.pid);
        printf("The queue id is %d\n", newnode->this.pid);
        fflush(stdout);
        return first;
    }
    return NULL;
}

void execute_process(long *num_processors) {
    if ((*num_processors) > 0) {
        if (first != NULL) {
            PCB pcb = first->this;
            node *current = first;
            if (first == last) {
                first = NULL;
                last = NULL;
            } else {
                first = first->next;
            }
            if (pthread_create(&pth[(*num_processors) - 1], NULL, &process, current) != 0)
                perror("Failed to create thread\n");
            (*num_processors)--;
            if (pthread_join(pth[(*num_processors)], NULL) != 0) perror("Failed to join thread\n");
            (*num_processors)++;
            if (current->this.live_time > 0) {
                // Volver a meterlo en la cola
                add_process(current);
            } else {
                free(current);
            }
        }
        // mirar si el tiempo de vida del proceso acabo y si no volver a meterlo en la cola
        // Cuando esto termine hay que incrementar el num_processors
    }
}

void *process(void *current) {
    // nanosleep()
    node *new_process = current;
    // printf("LIVE_TIME - PRE %d\n", new_process->this.live_time);
    new_process->this.live_time = new_process->this.live_time - new_process->this.quantum; // No se guarda el cambio en la variable live_time
    printf(" - Executing process %d...\n", new_process->this.pid);
    // printf("LIVE_TIME - POST %d\n", new_process->this.live_time);
    fflush(stdout);
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