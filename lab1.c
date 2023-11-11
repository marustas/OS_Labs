#include "stdlib.h"
#include "stdio.h"
#include <sys/types.h>
#include <unistd.h>
int main()
{
	// make two process which run same
	// program after this instruction
	pid_t pid = fork();
	if(pid == 0){
	printf("I'm a child process with the pid of %d\n", pid);
	}
	else if(pid > 0){
	printf("I'm a parent process of %d\n", getpid());
	}
	else{
	perror("fork fail");
	exit(1);
	}
	return 0;
}