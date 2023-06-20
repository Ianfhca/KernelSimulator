#include "../defines/scheduler.h"

void *timer2(void *arguments) {
    pthread_mutex_lock(&mutex);
    args_t *args = arguments;
    int pulses = 0;
    // printf("I'm the timer 1 with tid %d\n", gettid());
    while (1) {
        args->done++;
        pulses++;
        if (pulses == args->freq_schl) {
            pulses = 0;
            // if (num_processors >= 0) { // revisar que sea > o >=
            //     current = get_first();
            //     if (current != NULL) {
            //         printf("*LIVE_TIME - PRE %d\n", current->this.live_time);
            //         if (pthread_create(&pth[num_processors-1], NULL, &execute_process, current) != 0)
            //             perror("Failed to create thread\n");
            //         num_processors--;
            //         if (pthread_join(pth[num_processors], NULL) != 0) // Falta añadir el contador de ciclos
            //             perror("Failed to join thread\n");
            //         printf("*LIVE_TIME - POST %d\n", current->this.live_time);
            //         if (current->this.live_time > 0) {
            //             // Volver a meterlo en la cola
            //             add_process(current);
            //         } else {
            //             free(current);
            //             num_process--;
            //         }
            //         num_processors++;
            //     } else {
            //         args->finished = 2;
            //         printf("The simulation is has finished\n");
            //         exit(0);
            //     }
            printf("Timer 2 has interrupt\n");
            fflush(stdout);
            // }
        }
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond2, &mutex);
    }
    pthread_mutex_unlock(&mutex);
}