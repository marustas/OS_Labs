#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/wait.h>

#define MAX 10

/*
Structure of shared data:
VAR
Flag for terminating 
Mutex fo readers and writer
*/
typedef struct {
    int VAR;
    int read_count;
    int terminate;
    sem_t writer_mutex;
    sem_t reader_mutex;
} shared_data;

shared_data *shared_mem;

/*
Function for initial setup of the shared memory
Intitialize VAR to 0
Lower the terminate flag (set to 0)
*/
void init_shared_memory() {
    int fd = shm_open("/my_shared_memory", O_CREAT | O_RDWR, 0666);
    ftruncate(fd, sizeof(shared_data));
    shared_mem = (shared_data *)mmap(0, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    shared_mem->VAR = 0;
    shared_mem->read_count = 0;
    shared_mem->terminate = 0;
    sem_init(&shared_mem->writer_mutex, 1, 1);
    sem_init(&shared_mem->reader_mutex, 1, 1);
}

/*
Function for cleaning the shared memory resources:
Destroying the semaphore
Removing the mapping to the shareed memory
Removing the name of the shared memory object
*/
void cleanup_shared_memory() {
    sem_destroy(&shared_mem->writer_mutex);
    sem_destroy(&shared_mem->reader_mutex);
    munmap(shared_mem, sizeof(shared_data));
    shm_unlink("/my_shared_memory");
}

//The writer process
void writer_process() {
    //The writer will write the VAR until it reached MAX value
    while (shared_mem->VAR < MAX) {
        /*
        Lock the semaphore, so that the readers can't read
        The writer enters critical section
        */
        sem_wait(&shared_mem->writer_mutex);
        printf("The writer acquires the lock\n");
        shared_mem->VAR++;
        printf("The writer (%d) writes the value %d\n", getpid(), shared_mem->VAR);
        printf("The writer releases the lock\n");
        /*
        Unlock the semaphore, the critical section has ended
        The readers can now read the VAR
        */
        sem_post(&shared_mem->writer_mutex);
        //If the VAR is equal to MAX, the flag is raised(set to 1)
        if (shared_mem->VAR == MAX) {
            shared_mem->terminate = 1;
        }
        //Delay writer for messages in the terminal to be visible
        sleep(1); 
    }
    exit(0);
}

//Reader process
void reader_process(int sleep_time) {
    while (1) {
        //Check if the flag is raised
        if (shared_mem->terminate) {
            break;
        }
        /*
        Lock the semaphore, so that the writer can't write
        Both of the readers can read
        Enter the critical section
        Exclusive acces to reader_count
        */
        sem_wait(&shared_mem->reader_mutex);
        printf("The reader (%d) acquires the lock\n", getpid());
        //Check if it's the first reader and block the writer
        if (shared_mem->read_count == 1) {
            sem_wait(&shared_mem->writer_mutex);
        }
        //Unlock the reader semaphore to allow the 2nd reader into its critical section
        sem_post(&shared_mem->reader_mutex);

        printf("The reader (%d) reads the value %d\n", getpid(), shared_mem->VAR);

        //Lock the reader semaphore to safely update read_count
        sem_wait(&shared_mem->reader_mutex);
        shared_mem->read_count--;
        //Check if it's the last reader and release the writer to allow it into its critical section
        if (shared_mem->read_count == 0) {
            sem_post(&shared_mem->writer_mutex);
        }
        printf("The reader (%d) releases the lock\n", getpid());
       /*
        Unlock the semaphore, the critical section has ended
        */
        sem_post(&shared_mem->reader_mutex);
        //Delay the reader for designated amount of time
        usleep(sleep_time); 
    }
}

int main() {
    init_shared_memory();

    //Array with sleep times for readers
    int sleepreaderarr[2] = {500000, 1000000};
    //Create 2 reader processes
    for (int i = 0; i < 2; i++) {
        if (fork() == 0) {
            reader_process(sleepreaderarr[i]);
        }
    }
    pid_t writer_pid = fork();
    if (writer_pid == 0) {
        writer_process();
    }

    // Wait for the writer to finish
    waitpid(writer_pid, NULL, 0);
    for (int i = 0; i < 2; i++) {
        wait(NULL);
    }

    // Cleanup
    cleanup_shared_memory();
    return 0;
}