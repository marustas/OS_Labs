#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int ppipe[2];
    pid_t pid;

    // Create a pipe and exit if an error occurs
    if (pipe(ppipe) == -1) {
        perror("Failed to create a pipe");
        exit(EXIT_FAILURE);
    }

    // Create a child process and exit if an error occurs
    pid = fork();
    if (pid == -1) {
        perror("Failed to fork");
        exit(EXIT_FAILURE);
    }

	// Child process
    if (pid == 0) { 
        /*
        Close read end of the pipe,
        copy the write end of the pipe to STDOUT of the file descriptor
        and close it
        */
        close(ppipe[0]);
        dup2 (ppipe[1], STDOUT_FILENO);
        close(ppipe[1]);
        
        // Execute 'ls /' and exit if an error occurs
        if (execlp("ls", "ls", "/", NULL) == -1) {
        perror("Failed to execute");
        exit(EXIT_FAILURE);
        };
    } else {
        // Parent process 
        /*
        Close write end of the pipe,
        copy the read end of the pipe to STDIN of the file desriptor
        and close it
        */
        close(ppipe[1]);
        dup2 (ppipe[0], STDIN_FILENO);
        close(ppipe[0]);
        wait(NULL);

        // Execute 'wc -l' and exit if an error occurs
        if (execlp("wc", "wc", "-l", NULL) == -1) {
        perror("Failed to execute");
        exit(EXIT_FAILURE);
        };
    }
	return 0;
}