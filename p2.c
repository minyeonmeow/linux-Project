#define _GNU_SOURCE
#define STACK_SIZE (1024 * 1024)
#define SIZE 100

#include <stdio.h>
#include <sched.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include "header.h"

int test_function(void *ptr) {
	char *(arguments[2]);
	unsigned int i, counter, buffer[SIZE];

	counter = get_process_session_group(buffer, SIZE);
	if (counter) {
		printf("What follows are the local PIDs of the processes that are in the same login sesson of this process.\n");         
		for (i=0; i<counter; ++i) printf("[%d] %u\n", i, buffer[i]);
	} else {
		printf("There is an error when executing this system call.\n");
	}
	
	puts("=== check ===\n");
	arguments[0] = "-T";
	arguments[1] = NULL;
	execve("/bin/ps", arguments, NULL);

	return 0;
}

int main(void) {
	char *stack;
	unsigned int chpid;
	
	stack = malloc(STACK_SIZE);
	chpid = clone(test_function, stack + STACK_SIZE, CLONE_NEWPID | SIGCHLD, NULL);

	waitpid(chpid, NULL, 0);
	free(stack);

	return 0;
}
