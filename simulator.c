#include "structs.h" // structs args_t & PCB

#define MAX_TIMERS 2
#define NUM_THREADS MAX_TIMERS+1

long num_processors;

pthread_mutex_t mutex;
pthread_cond_t cond1;
pthread_cond_t cond2;
int done;

int num_process = 0;
int max_process = 200;

PCB * pcb_queue;
pthread_t pth[12];

node *first = NULL;
node *last = NULL;

void generate_process() {
    if (num_process < 200) {
        node *newnode = malloc(sizeof(node));
        newnode->this.tid = num_process;
        newnode->this.status = 0;
        newnode->this.live_time = rand() % 100 + 20;
        newnode->this.quantum = 2;
        newnode->next = NULL;
        // PCB pcb;
        // pcb.tid = num_process;
        // pcb.status = 0;
        // pcb.live_time = rand() % 100 + 20; // revisar random entre 20 y 100
        // pcb.quantum = 2;
        //pcb_queue[max_process] = pcb;
        num_process++;
        //max_process--;
        printf("EL IDENTIFICADOR DE LA COLA %d\n", newnode->this.tid);
        //printf("\nSe ha generado el proceso con id %d\n", pcb.tid);
        fflush(stdout);
    }
 }

 void * process() {
    // nanosleep()
    int tid = gettid();
    printf("Executing process %d", gettid());
    // if (pthread_join(th[i], NULL) != 0) perror("Failed to join thread\n");
 }

 void execute_process() {
    if (num_processors > 0) {
        if (pthread_create(&pth[num_processors], NULL, &process, NULL) != 0) perror("Failed to create thread\n");
        num_processors--;
        // pcb_queue[max_process];
    }
 }

 void * cpu_clock(void * arg) {
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

 void * timer1(void * arg) {
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
            execute_process();
        }
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond2, &mutex);
    }
 }

  void * timer2(void * arg) {
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

int main(int argc, char * argv[]) {
    num_processors = sysconf(_SC_NPROCESSORS_ONLN);
    printf("The number of processors is %ld\n", num_processors);

    pthread_t th[NUM_THREADS];
    args_t args;
    args.cycles1 = 0;
    args.cycles2 = 0;
    args.count1 = 10000;
    args.count2 = 50000;

    pcb_queue = (PCB*) malloc(200 * sizeof(PCB));
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