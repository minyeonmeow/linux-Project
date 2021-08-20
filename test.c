#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "page_info.h"

#define MEMORY_SIZE 10000

/* main function*/
int main(void) {
    int result_1[MEMORY_SIZE];
    int result_2[MEMORY_SIZE];
    int result_3[MEMORY_SIZE];
	int exit_status;

	if (fork()) { // parent process
        linux_survey_TT((char*) result_1);

        write_buffer(result_1, "./result_1.txt");

		wait(&exit_status);
    } else { // child process
        linux_survey_TT((char*) result_2);

        result_1[0] = 123;

        linux_survey_TT((char*) result_3);

        write_buffer(result_2, "./result_2.txt");
        write_buffer(result_3, "./result_3.txt");
    }

	printf("pid: %d, result_1 addr: %p\n", getpid(), result_1);
	printf("pid: %d, result_2 addr: %p\n", getpid(), result_2);
	printf("pid: %d, result_3 addr: %p\n", getpid(), result_3);

    return 0;
}
/* end main     */
