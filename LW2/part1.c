#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 3
#define NUM_INCREMENTS 15

int buffer = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* increment_buffer(void* thread_id) {
    int tid = *((int*)thread_id);
    int changes = 0;

    for (int i = 0; i < NUM_INCREMENTS; ++i) {
        // Lock the mutex before entering the critical section
        pthread_mutex_lock(&mutex);

        // Critical section
        printf("TID: %d, PID: %ld, Buffer: %d\n", tid, (long)getpid(), buffer);
        buffer++;
        changes++;

        // Unlock the mutex after exiting the critical section
        pthread_mutex_unlock(&mutex);
    }

    // Return the number of changes made by the thread
    return (void*)changes;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    // Create threads
    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_ids[i] = i;
        if (pthread_create(&threads[i], NULL, increment_buffer, (void*)&thread_ids[i]) != 0) {
            perror("Error creating thread");
            exit(EXIT_FAILURE);
        }
    }

    int total_changes = 0;

    // Wait for threads to finish and collect the number of changes from each thread
    for (int i = 0; i < NUM_THREADS; ++i) {
        void* changes;
        if (pthread_join(threads[i], &changes) != 0) {
            perror("Error joining thread");
            exit(EXIT_FAILURE);
        }

        total_changes += (int)changes;
    }

    // Print the total changes made by all threads
    printf("\nTotal changes made by all threads: %d\n", total_changes);

    return 0;
}