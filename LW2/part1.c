#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 3
#define TOTAL_OPERATIONS 15

int buffer = 0;
int counter = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* increment_buffer(void* thread_id) {
    int tid = *((int*)thread_id);
    int changes = 0;

    while (1) {
        pthread_mutex_lock(&mutex);

        if (counter < TOTAL_OPERATIONS) {
            if (counter % NUM_THREADS == tid) {
                printf("TID: %lu, PID: %ld, Buffer: %d\n", pthread_self(), (long)getpid(), buffer);
                buffer++;
                changes++;
                counter++;
            }
        }

        pthread_mutex_unlock(&mutex);

        if (counter >= TOTAL_OPERATIONS) {
            break;
        }
    }

    return (void*)changes;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_ids[i] = i;
        if (pthread_create(&threads[i], NULL, increment_buffer, (void*)&thread_ids[i]) != 0) {
            perror("Error creating thread");
            exit(EXIT_FAILURE);
        }
    }

    int total_changes = 0;
    int changes_by_thread[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; ++i) {
        void* changes;
        if (pthread_join(threads[i], &changes) != 0) {
            perror("Error joining thread");
            exit(EXIT_FAILURE);
        }

        changes_by_thread[i] = (int)changes;
        total_changes += changes_by_thread[i];
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        printf("TID %lu worked on the buffer %d times\n", threads[i], changes_by_thread[i]);
    }

    printf("Total changes made by all threads: %d\n", total_changes);

    return 0;
}
