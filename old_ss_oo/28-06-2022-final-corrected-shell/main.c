#include "exec_line.h"
#include "exec_commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <err.h>

int
main(int argc, char *argv[])
{
	char string[MAX];
	char end_string[] = "EXIT";
	int end = 0, i, background = 0, get_stdout = dup(1), get_stdin = dup(0);
	int times_executed;

	times_executed = 0;

	close(WRITE_END);
	dup(get_stdout);	// designate the stdoutput, the terminal
	close(READ_END);
	dup(get_stdin);		// designate the stdoutput, the terminal

	printf("Shell made by Julia López Augusto\n");
	printf("Write EXIT to leave\n");
	printf("# ");		// prints the prompt

	//read maximun of 1023 characters 
	while (fgets(string, MAX - 1, stdin) != NULL) {
		string[strcspn(string, "\r\n")] = 0;

		//Look if the input has a background symbol &
		background = 0;
		for (i = 0; string[i] != '\0'; i++) {

			if (string[i] == '&') {
				string[i] = '\0';
				background = 1;
			}
		}
		end = equal_strings(string, end_string);	// if string is EXIT, get out of the while
		process_output(string, background, times_executed);	// if not, gets into command function

		if (end != 0)
			break;

		close(WRITE_END);
		dup(get_stdout);	// designate the stdoutput, the terminal
		close(READ_END);
		dup(get_stdin);	// designate the stdoutput, the terminal
		printf("# ");	// prints the Prompt
		times_executed++;
	}
	exit(EXIT_SUCCESS);	// end of the program (if string 'EXIT' introduced)

}
