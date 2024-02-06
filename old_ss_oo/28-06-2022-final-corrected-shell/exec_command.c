#include "exec_command.h"
#include "exec_commands.h"
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <err.h>
#include <stdio.h>

// just execute one command
void
process_command(char *string, int background)
{
	struct Command command;
	int length_word;
	int length_first_arg, total_length1, total_length2, length_static_path1,
	    length_static_path2;
	char *path1, *path2;
	int pid, status;
	char st[20];

	length_word = strlen(string);

	command.args = malloc((length_word + 5) * sizeof(char *));
	token_arg(string, command.args);

	path1 = "/bin/";
	path2 = "/usr/bin";

	length_first_arg = strlen(command.args[0]);
	length_static_path1 = strlen(path1);
	length_static_path2 = strlen(path2);

	total_length1 = length_first_arg + length_static_path1;
	total_length2 = length_first_arg + length_static_path2;
	command.new_path1 = malloc(sizeof(char) * (total_length1 + 1));
	command.new_path2 = malloc(sizeof(char) * (total_length2 + 1));
	strcat(strcpy(command.new_path1, path1), command.args[0]);
	strcat(strcpy(command.new_path2, path2), command.args[0]);

	pid = fork();

	switch (pid) {
	case -1:
		free(command.new_path1);
		free(command.new_path2);
		free(command.args);
		err(EXIT_FAILURE, "cannot fork");
		break;
	case 0:
		execv(command.new_path1, command.args);
		execv(command.new_path2, command.args);

		free(command.new_path1);
		free(command.new_path2);
		free(command.args);
		err(EXIT_FAILURE, "exec failed");
		break;
	default:		//father
		if (background == 0) {
			wait(&status);
			sprintf(st, "%d", WEXITSTATUS(status));
			setenv("RESULT", st, 1);
		}
		free(command.args);
		free(command.new_path1);
		free(command.new_path2);
	}
}
