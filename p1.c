#include <stdio.h>
#include <stdlib.h>
#include "header.h"

#define SIZE 100

int main(void) {
	int i, counter;
	unsigned int results[SIZE];

	counter = get_process_zero_session_group(results, SIZE);

	if (counter) {
		printf("What follows are the PIDs of the processes that are in the same login sesson of process 0.\n");         
		for (i=0; i<counter; ++i) {
			printf("[%d] %u\n", i, results[i]);
		}
	} else {
		printf("There is an error when executing this system call.\n");
	}
	
	puts("=== check ===");
	system("ps -A -o \'user,pid,sess\' | grep -E \'root.* 0\'");

	return 0;
}
