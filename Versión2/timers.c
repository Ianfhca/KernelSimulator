/**
 * Function that generates processes with random properties
 * void *arg is an struct
*/
void *timer1(void *arg) {
    pthread_mutex_lock(&mutex);
    args_t *args = arg;
    printf("I'm the timer 2 with tid %d\n", gettid());
    while (1) {
        args->done++;
        args->cycles2++;
        if (args->cycles2 == args->count1) {
            args->cycles2 = 0;
            if (num_process < MAX_PROCESS && args->finished == 0) {
                newprocess = generate_process(num_process);
                add_process(newprocess);
                num_process++;
            } else {
                args->finished = 1;
            }
        }
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond2, &mutex);       
    }
    pthread_mutex_unlock(&mutex);
}

/**
 * Function 
 * 
*/
void *timer2(void *arg) {
    pthread_mutex_lock(&mutex);
    args_t *args = arg;
    printf("I'm the timer 1 with tid %d\n", gettid());
    while (1) {
        args->done++;
        args->cycles1++;
        if (args->cycles1 == args->count2) {
            args->cycles1 = 0;
            if (num_processors >= 0) { // revisar que sea > o >=
                current = get_first();
                if (current != NULL) {
                    printf("*LIVE_TIME - PRE %d\n", current->this.live_time);
                    if (pthread_create(&pth[num_processors-1], NULL, &execute_process, current) != 0)
                        perror("Failed to create thread\n");
                    num_processors--;
                    if (pthread_join(pth[num_processors], NULL) != 0) // Falta añadir el contador de ciclos
                        perror("Failed to join thread\n");
                    printf("*LIVE_TIME - POST %d\n", current->this.live_time);
                    if (current->this.live_time > 0) {
                        // Volver a meterlo en la cola
                        add_process(current);
                    } else {
                        free(current);
                        num_process--;
                    }
                    num_processors++;
                } else {
                    args->finished = 2;
                    printf("The simulation is has finished\n");
                    exit(0);
                }
            }
        }
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond2, &mutex);
    }
    pthread_mutex_unlock(&mutex);
}