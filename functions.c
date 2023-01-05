#include "./functions.h"

//long num_processors;
int num_process = 0;

// PCB * pcb_queue;
pthread_t pth[12];

node *first = NULL;
node *last = NULL;

void generate_process() {
    if (num_process < MAX_PROCESS) {
        node *newnode = malloc(sizeof(node));
        newnode->this.pid = num_process;
        newnode->this.status = 0;
        newnode->this.live_time = (rand() % 80) + 20; // random value between 20 and 100
        newnode->this.quantum = 2;
        newnode->next = NULL;
        if (first == NULL) {
            first = newnode;
            last = newnode;
        } else {
            last->next = newnode;
            last = newnode;
        }
        num_process++;
        // printf("\nSe ha generado el proceso con id %d\n", pcb.pid);
        printf("The queue id is %d\n", newnode->this.pid);
        fflush(stdout);
    }
}

void *process(void *pcb) {
    // nanosleep()
    PCB *current = pcb;
    printf(" - Executing process %d...\n", current->pid);
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
            // printf("%d\n", *num_processors);
            // (*num_processors)--;
            // (*num_processors) = (*num_processors) - 1;
            if (pthread_create(&pth[(*num_processors) - 1], NULL, &process, &pcb) != 0)
                perror("Failed to create thread\n");
            (*num_processors)--;
            
            if (pthread_join(pth[(*num_processors)], NULL) != 0) perror("Failed to join thread\n");
            if (pcb.live_time > 0) {
                // Volver a meterlo en la cola
            } else {
                free(current);
            }
        }
        // mirar si el tiempo de vida del proceso acabo y si no volver a meterlo en la cola
        // Cuando esto termine hay que incrementar el num_processors
    }
}