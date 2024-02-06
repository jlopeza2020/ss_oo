#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

enum {
	MIN_ARGS = 2,
};

void
usage()
{
	fprintf(stderr, "usage: execargs secs commands [command ..] \n");
	exit(EXIT_FAILURE);
}

void
token_arg(char *cmd, char **wrds_exec)
{
	char *token;
	char *rest = cmd;
	int i = 0;

	while ((token = strtok_r(rest, " ", &rest))) {
		wrds_exec[i] = token;
		i++;
	}
}

void
father_process(char **words_exec, int status)
{
	//wait until child process finishes
	wait(&status);

	if (WIFEXITED(status) != 0) {
		if (WEXITSTATUS(status) > 0) {
			fprintf(stderr, "error: %s\n", words_exec[0]);
			free(words_exec);
			exit(EXIT_FAILURE);
		}
	}
}

void
son_process(char **words_exec)
{
	execv(words_exec[0], words_exec);

	//if you get to this point means that execv went wrong, free memory
	fprintf(stderr, "%s : %s\n", words_exec[0], strerror(errno));
	free(words_exec);
	exit(EXIT_FAILURE);

}

void
exec_father_son(char *cmd)
{
	pid_t child_pid;
	int status = 0;

	int lon_palabra = strlen(cmd);

	//allocate memory for the new array which contains all the tokens of the argument
	char **wrds_exec = malloc((lon_palabra + 1) * sizeof(char *));

	token_arg(cmd, wrds_exec);

	child_pid = fork();

	if (child_pid > 0) {
		father_process(wrds_exec, status);

	} else if (child_pid == 0) {
		son_process(wrds_exec);

	} else {
		fprintf(stderr, "Error in fork call \n");
		exit(EXIT_FAILURE);
	}

}

long
is_lnguint(char **args_values, int first_pos)
{
	char *ptr;
	long ret;

	ret = strtol(args_values[first_pos], &ptr, 10);

	if (ptr[0] != '\0' || ret < 0)
		usage();

	return ret;
}

void
print_output(int amount_args, char **args_values, int first_pos, int second_pos)
{
	long time;
	int current_pos;

	time = is_lnguint(args_values, first_pos);

	for (current_pos = second_pos; current_pos < amount_args; current_pos++) {

		exec_father_son(args_values[current_pos]);
		sleep(time);
	}

}

int
main(int argc, char *argv[])
{
	int first_exec_pos = 1;
	int second_exec_pos = 2;

	if (argc > MIN_ARGS) {
		print_output(argc, argv, first_exec_pos, second_exec_pos);
		exit(EXIT_SUCCESS);
	} else
		usage();

}
