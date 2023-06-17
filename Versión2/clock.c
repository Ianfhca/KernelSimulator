#include "structs.h"
/**
 * Funtion to symulate CPU clock frecuency
 * 
*/
void *cpu_clock(void *arg) {
    args_t *args = arg;
    printf("Im the clock with tid %d\n", gettid());
    while (1) {
        pthread_mutex_lock(&mutex);
        while (args->done < args->num_timers) {
            pthread_cond_wait(&cond1, &mutex); // Unlocks the mutex, waits for signal and it locks again
        }
        args->done = 0;
        pthread_cond_broadcast(&cond2);
        pthread_mutex_unlock(&mutex);
    }
}