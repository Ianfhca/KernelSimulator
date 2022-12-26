#include "./functions.h"

//long num_processors;
int num_process = 0;

// PCB * pcb_queue;
pthread_t pth[12];

node *first = NULL;
node *last = NULL;

void generate_process()
{
    if (num_process < MAX_PROCESS)
    {
        node *newnode = malloc(sizeof(node));
        newnode->this.tid = num_process;
        newnode->this.status = 0;
        newnode->this.live_time = rand() % 100 + 20; // random value between 20 and 100
        newnode->this.quantum = 2;
        newnode->next = NULL;
        if (first == NULL)
        {
            first = newnode;
            last = newnode;
        } else {
            last->next = newnode;
            last = newnode;
        }
        num_process++;
        // printf("\nSe ha generado el proceso con id %d\n", pcb.tid);
        printf("EL IDENTIFICADOR DE LA COLA %d\n", newnode->this.tid);
        fflush(stdout);
    }
}

void *process(void *pcb)
{
    // nanosleep()
    int tid = gettid();
    printf(" - Executing process %d\n", gettid());
    // if (pthread_join(th[i], NULL) != 0) perror("Failed to join thread\n");
}

void execute_process(long num_processors)
{
    if (num_processors > 0)
    {
        if (first != NULL)
        {
            PCB pcb = first->this;
            node *current = first;
            if (first == last)
            {
                first = NULL;
                last = NULL;
            } else {
                first = first->next;
            }
            free(current);
            if (pthread_create(&pth[num_processors - 1], NULL, &process, &pcb) != 0)
                perror("Failed to create thread\n");
            num_processors--;
        }
        // mirar si el tiempo de vida del proceso acabo y si no volver a meterlo en la cola
    }
}