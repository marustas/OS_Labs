#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int pipefd[2];
    pid_t pid;

    // Create a pipe
    if (pipe(pipefd) == -1) {
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
    close(pipefd[0]);
    dup2 (pipefd[1], STDOUT_FILENO);
    close(pipefd[1]);
	
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
    close(pipefd[1]);
    dup2 (pipefd[0], STDIN_FILENO);
    close(pipefd[0]);
	wait(NULL);

    // Execute 'wc -l'
    execlp("wc", "wc", "-l", NULL);

    // Same as in Child process
    perror("execlp");
    exit(EXIT_FAILURE);
    }
	return 0;
}