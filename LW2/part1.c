#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

//Set the number of threads and buffer increments
#define NUM_THREADS 3
#define TOTAL_INCREMENTS 15
//buffer and counter are initialized as shared variables
int buffer = 0;
int counter = 0;
//Initializing mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int random_sleep() {
    // Generate a random sleep number
    return (rand() % 1000);
}

//This is the work performed on the buffer, that is incrementing
void* increment_buffer(void* thread_id) {
    int tid = *((int*)thread_id);
    int changes = 0;

    while (1) {
        //Lock the mutex object
        pthread_mutex_lock(&mutex);
        //The counter is incremented until it reaches the desired number of total operations(15)
        if (counter < TOTAL_INCREMENTS) {
            //Print the Thread ID, process ID and the value of the buffer
            printf("TID: %lu, PID: %ld, Buffer: %d\n", pthread_self(), (long)getpid(), buffer);
            //Increment the value of the buffer
            buffer++;
            //Increment the number of changes made by the thread
            changes++;
            //Increment counter 
            counter++;
        }
        //Lock the mutex object
        pthread_mutex_unlock(&mutex);

        //Delay for a generated amount of time
        usleep(random_sleep());
        //Check if the value of the counter violates the limit of total operation
        if (counter >= TOTAL_INCREMENTS) {
            break;
        }
    }
    //Return the number of changes
    return (void*)changes;
}

int main() {
    //Initialize 3 threads
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    //Thread creation and error handling
    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_ids[i] = i;
        if (pthread_create(&threads[i], NULL, increment_buffer, (void*)&thread_ids[i]) != 0) {
            perror("Error creating thread");
            exit(EXIT_FAILURE);
        }
    }
    /*
    Start counting the total number of changes made to the buffer variable and the amount of work done by each thread
    */
    int total_changes = 0;
    int changes_by_thread[NUM_THREADS];
    /*
    Wait for the threads to finish execution
    Collect the number of increments done by the thread
    Join into the main thread
    Handle possible errors in thread joinment
    */
    for (int i = 0; i < NUM_THREADS; ++i) {
        void* changes;
        if (pthread_join(threads[i], &changes) != 0) {
            perror("Error joining thread");
            exit(EXIT_FAILURE);
        }
        //Sum individual changes
        changes_by_thread[i] = (int)changes;
        total_changes += changes_by_thread[i];
    }
    //Final output
    for (int i = 0; i < NUM_THREADS; ++i) {
        printf("TID %lu worked on the buffer %d times\n", threads[i], changes_by_thread[i]);
    }

    printf("Total buffer accesses: %d\n", total_changes);

    return 0;
}
