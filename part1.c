#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int ppipe[2];
    pid_t pid;

    // Create a pipe
    if (pipe(ppipe) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Create a child process
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

	// Child process
    if (pid == 0) { 
	/*
	Close read end of the pipe,
	put STOUT into the write end of the pipe and close it
	 */
    close(ppipe[0]);
    dup2 (ppipe[1], STDOUT_FILENO);
    close(ppipe[1]);
	
    // Execute 'ls /'
    execlp("ls", "ls", "/", NULL);

    // If an error then no return
    perror("execlp");
    exit(EXIT_FAILURE);
    } else {
    // Parent process 

	/*
	Close write end of the pipe,
	put STDIN into the read end of the pipe and close it
	 */
    close(ppipe[1]);
    dup2 (ppipe[0], STDIN_FILENO);
    close(ppipe[0]);
	wait(NULL);

    // Execute 'wc -l'
    execlp("wc", "wc", "-l", NULL);

    // Same as in Child process
    perror("execlp");
    exit(EXIT_FAILURE);
    }
	return 0;
}