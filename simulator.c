// #include "structs.h" // structs args_t & PCB
#include "./functions.h"

long num_processors;

pthread_mutex_t mutex;
pthread_cond_t cond1;
pthread_cond_t cond2;
int done;

 void * cpu_clock(void *arg) {
    printf("Im the clock\n");
    while (1) {
        pthread_mutex_lock(&mutex);
        while (done < MAX_TIMERS) {
            pthread_cond_wait(&cond1, &mutex); //unlocks the mutex, wait for signal and locks again
        }
        done = 0;
        pthread_cond_broadcast(&cond2);
        pthread_mutex_unlock(&mutex);
    }
 }

 void * timer1(void *arg) {
    pthread_mutex_lock(&mutex);
    printf("Im the timer 1\n");
    // count = 20000; 
    args_t *args = arg;
    args->pcb.tid = gettid();
    printf("I'm tid %d\n", args->pcb.tid);
    while (1) {
        done++;
        args->cycles1++;
        if (args->cycles1 == args->count2) {
            //printf("\n%d ha contado %d ciclos\n", gettid(), args->cycles1);
            //fflush(stdout);
            args->cycles1 = 0;
            execute_process(num_processors);
        }
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond2, &mutex);
    }
 }

  void * timer2(void *arg) {
    pthread_mutex_lock(&mutex);
    printf("I'm the timer 2\n");
    // count = 20000; 
    args_t *args = arg;
    args->pcb.tid = gettid();
    printf("Im tid %d\n", args->pcb.tid);
    while (1) {
        done++;
        args->cycles2++;
        if (args->cycles2 == args->count1) {
            // printf("\n%d ha contado %d ciclos\n", gettid(), args->cycles2);
            // fflush(stdout);
            args->cycles2 = 0;
            generate_process();
        }
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond2, &mutex);
    }
 }

int main(int argc, char *argv[]) {
    num_processors = sysconf(_SC_NPROCESSORS_ONLN);
    printf("The number of processors is %ld\n", num_processors);

    pthread_t th[NUM_THREADS];
    args_t args;
    args.cycles1 = 0;
    args.cycles2 = 0;
    args.count1 = 10000;
    args.count2 = 50000;

    // pcb_queue = (PCB*) malloc(200 * sizeof(PCB));
    srand(time(NULL));

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond1, NULL);
    pthread_cond_init(&cond2, NULL);
    if (pthread_create(&th[0], NULL, &cpu_clock, NULL) != 0) perror("Failed to create th1\n");
    if (pthread_create(&th[1], NULL, &timer1, &args) != 0) perror("Failed to create th2\n");
    if (pthread_create(&th[2], NULL, &timer2, &args) != 0) perror("Failed to create th3\n");

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(th[i], NULL) != 0) perror("Failed to join thread\n");
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond1);
    pthread_cond_destroy(&cond2);
    return 0;
}

// ------------------------------------------------------ MORRALLA -----------------------------------------------------------------
// int num_process = 0;

// // PCB * pcb_queue;
// pthread_t pth[12];

// node *first = NULL;
// node *last = NULL;

// void generate_process() {
//     if (num_process < MAX_PROCESS) {
//         node *newnode = malloc(sizeof(node));
//         newnode->this.tid = num_process;
//         newnode->this.status = 0;
//         newnode->this.live_time = rand() % 100 + 20; // random value between 20 and 100
//         newnode->this.quantum = 2;
//         newnode->next = NULL;
//         if (first == NULL) {
//             first = newnode;
//             last = newnode;
//         } else {
//             last->next = newnode;
//             last = newnode;
//         }
//         num_process++;
//         // printf("\nSe ha generado el proceso con id %d\n", pcb.tid);
//         printf("EL IDENTIFICADOR DE LA COLA %d\n", newnode->this.tid);
//         fflush(stdout);
//     }
//  }

//  void * process(void *pcb) {
//     // nanosleep()
//     int tid = gettid();
//     printf(" - Executing process %d\n", gettid());
//     // if (pthread_join(th[i], NULL) != 0) perror("Failed to join thread\n");
//  }

//  void execute_process() {
//     if (num_processors > 0) {
//         if (first != NULL) {
//             PCB pcb = first->this;
//             node *current = first;
//             if (first == last) {
//                 first = NULL;
//                 last = NULL;
//             } else {
//                 first = first->next;
//             }
//             free(current);
//             if (pthread_create(&pth[num_processors-1], NULL, &process, &pcb) != 0) perror("Failed to create thread\n");
//             num_processors--;
//         }
//         //mirar si el tiempo de vida del proceso acabo y si no volver a meterlo en la cola
//     }
//  }