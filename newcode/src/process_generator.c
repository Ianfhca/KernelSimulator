#include "../defines/process_generator.h"

/**
 * This function counts clock pulses and generates a processe when pulses reach established frecuence.
 * Parameters:
 *  - *arg: arg_t type struct
 * Return:
 */
void *timer1(void *arg) {
    pthread_mutex_lock(&mutex);
    args_t *args = arg;
    int pulses = 0;
    printf("I'm the timer 2 with tid %d\n", gettid());
    while (1) {
        args->done++;
        pulses++;
        if (pulses == args->freq_pgen) {
            pulses = 0;
            // if (num_process < MAX_PROCESS && args->finished == 0) {
            //     newprocess = generate_process(num_process);
            //     add_process(newprocess);
            //     num_process++;
            // } else {
            //     args->finished = 1;
            // }
            printf("Timer 1 has interrupt\n");
        }
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond2, &mutex);
    }
    pthread_mutex_unlock(&mutex);
}