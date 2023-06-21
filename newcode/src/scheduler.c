#include "scheduler.h"

void *timer2(void *arguments) {
    pthread_mutex_lock(&mutex);
    args_t *args = arguments;
    int pulses = 0;

    while (1) {
        args->done++;
        pulses++;
        if (pulses == args->freq_schl) {
            pulses = 0;
            printf("Scheduling time\n");
            fflush(stdout);
        }
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond2, &mutex);
    }
    pthread_mutex_unlock(&mutex);
}