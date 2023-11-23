#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/wait.h>

#define MAX 10

typedef struct
{
    int VAR;
    int read_count;
    int terminate;
    sem_t writer_mutex;
    sem_t reader_mutex;
} shared_data;

shared_data *shared_mem;

void init_shared_memory()
{
    int fd = shm_open("/my_shared_memory", O_CREAT | O_RDWR, 0666);
    ftruncate(fd, sizeof(shared_data));
    shared_mem = (shared_data *)mmap(0, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    shared_mem->VAR = 0;
    shared_mem->read_count = 0;
    shared_mem->terminate = 0;
    sem_init(&shared_mem->writer_mutex, 1, 1);
    sem_init(&shared_mem->reader_mutex, 1, 1);
}

void cleanup_shared_memory()
{
    sem_destroy(&shared_mem->writer_mutex);
    sem_destroy(&shared_mem->reader_mutex);
    munmap(shared_mem, sizeof(shared_data));
    shm_unlink("/my_shared_memory");
}

void writer_process()
{
    while (shared_mem->VAR < MAX)
    {
        sem_wait(&shared_mem->writer_mutex);
        shared_mem->VAR++;
        if(shared_mem->VAR == MAX){
            printf("Writer (PID %d) writes the MAX value\n", getpid());
        } else {
            printf("Writer (PID %d) writes the value %d\n", getpid(), shared_mem->VAR);
        }
        sem_post(&shared_mem->writer_mutex);
        if (shared_mem->VAR == MAX)
        {
            shared_mem->terminate = 1;
        }
        sleep(1); // some writing delay
    }
    exit(0);
}

void reader_process(int sleep_time)
{
    while (1)
    {
        if (shared_mem->terminate)
        {
            break;
        }
        sem_wait(&shared_mem->reader_mutex);
        shared_mem->read_count++;
        if (shared_mem->read_count == 1)
            sem_wait(&shared_mem->writer_mutex);
        sem_post(&shared_mem->reader_mutex);

        printf("Reader (PID %d) reads the value %d\n", getpid(), shared_mem->VAR);

        sem_wait(&shared_mem->reader_mutex);
        shared_mem->read_count--;
        if (shared_mem->read_count == 0)
            sem_post(&shared_mem->writer_mutex);
        sem_post(&shared_mem->reader_mutex);

        usleep(sleep_time); // some reading delay
    }
}

int main()
{
    init_shared_memory();

    int sleepreaderarr[2] = {750000, 1500000};
    for (int i = 0; i < 2; i++)
    {
        if (fork() == 0)
        {
            reader_process(sleepreaderarr[i]);
        }
    }

    pid_t writer_pid = fork();
    if (writer_pid == 0)
    {
        writer_process();
    }

    waitpid(writer_pid, NULL, 0);
    for (int i = 0; i < 2; i++)
    {
        wait(NULL);
    }

    cleanup_shared_memory();
    return 0;
}