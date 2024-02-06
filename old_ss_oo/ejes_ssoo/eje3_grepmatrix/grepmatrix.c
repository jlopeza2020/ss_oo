#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

enum {
	MIN_ARGS = 3,
};

void
usage(char *program_name)
{
	fprintf(stderr, "usage: %s word [words ...] -f file [file ...] \n",
		program_name);
	exit(EXIT_FAILURE);
}

int
is_fmodifier(char **args_values, int amount_args, int first_pos)
{
	int current_pos;
	char *p = "-f";
	int count_fmod = 0;

	for (current_pos = first_pos; current_pos < amount_args; current_pos++) {

		if (strcmp(args_values[current_pos], p) == 0) {
			count_fmod++;
		}
	}
	return count_fmod;
}

int
is_other_modifier(char **args_values, int amount_args, int first_pos)
{
	int current_pos;
	char minus = '-';
	char f = 'f';
	int count_mod = 0;

	for (current_pos = first_pos; current_pos < amount_args; current_pos++) {

		if (args_values[current_pos][0] == minus
		    && args_values[current_pos][1] != f) {
			count_mod++;
		} else if (args_values[current_pos][0] == minus
			   && args_values[current_pos][1] == f
			   && strlen(args_values[current_pos]) > 2) {
			count_mod++;
		}
	}

	return count_mod;

}

int
get_pos_fmodifier(char **args_values, int amount_args, int first_pos)
{
	int current_pos;
	char *p = "-f";

	for (current_pos = first_pos; current_pos < amount_args; current_pos++) {

		if (strcmp(args_values[current_pos], p) == 0) {
			break;
		}
	}
	return current_pos;
}

void
print_heading_words(char **args_values, int first_pos, int pos_fmod)
{
	int current_pos;

	for (current_pos = first_pos; current_pos < pos_fmod; current_pos++) {
		printf("\"%s\" ", args_values[current_pos]);
	}
	printf("\n");
}

void
free_memory(char **matrix, int num_words)
{
	for (int pos = 0; pos < num_words; pos++) {
		free(matrix[pos]);
	}
	free(matrix);
}

void
father_process(int status, char **matrix, int words_pos, char *file,
	       int file_pos, int num_words)
{

	//wait until child process finishes
	wait(&status);
	if (WIFEXITED(status) != 0) {
		if (WEXITSTATUS(status) == 1) {

			matrix[file_pos][words_pos] = 'o';

		} else if (WEXITSTATUS(status) == 2) {
			fprintf(stderr, "Error processing file: %s \n", file);
			free_memory(matrix, num_words);
			exit(EXIT_FAILURE);

		} else if (WEXITSTATUS(status) == 0) {
			matrix[file_pos][words_pos] = 'x';
		}

	}

}

void
son_process(char *word, char *file, char **matrix, int num_words)
{

	char *fgrep_arguments[] =
	    { "/usr/bin/fgrep", "-q", "-s", word, file, NULL };
	execv(fgrep_arguments[0], fgrep_arguments);

	//if you get to this point means that execv went wrong, free memory
	fprintf(stderr, "Error processing fgrep %s : %s\n", file,
		strerror(errno));
	free_memory(matrix, num_words);
	exit(EXIT_FAILURE);
}

void
fgrep(char *word, char *file, char **matrix, int words_pos, int file_pos,
      int num_words)
{

	pid_t child_pid;

	int status = 0;

	child_pid = fork();

	if (child_pid > 0) {
		father_process(status, matrix, words_pos, file, file_pos,
			       num_words);

	} else if (child_pid == 0) {

		son_process(word, file, matrix, num_words);

	} else {
		fprintf(stderr, "Error in fork call \n");
		exit(EXIT_FAILURE);
	}

}

void
exec_fgrep(char **args_values, int amount_args, int first_pos, int num_files,
	   int f_pos, char **matrix, int pos, int total_num_wrds)
{

	int words_pos;
	int file_pos;

	//ranks
	for (file_pos = 0; file_pos < num_files; file_pos++) {

		//columns
		for (words_pos = 0; words_pos < f_pos - 1; words_pos++) {
			fgrep(args_values[words_pos + 1],
			      args_values[file_pos + f_pos + 1], matrix,
			      words_pos, file_pos, total_num_wrds);
		}
	}
}

void
print_output(char **args_values, int amount_args, int first_pos)
{

	int f_pos = get_pos_fmodifier(args_values, amount_args, first_pos);
	int file_pos;
	int words_pos;
	int num_files;
	int pos;

	int total_num_wrds;

	total_num_wrds = f_pos - first_pos;

	//create bidimensional array

	//ranks
	num_files = amount_args - f_pos - 1;

	char **matrix = malloc(num_files * sizeof(char **));

	//columns
	for (pos = 0; pos < total_num_wrds; pos++) {
		matrix[pos] = malloc(sizeof(char *));
	}

	exec_fgrep(args_values, amount_args, first_pos, num_files, f_pos,
		   matrix, pos, total_num_wrds);

	//print the real output
	print_heading_words(args_values, first_pos, f_pos);
	//ranks
	for (file_pos = 0; file_pos < num_files; file_pos++) {

		//columns
		for (words_pos = 0; words_pos < f_pos - 1; words_pos++) {
			printf("%c \t", matrix[file_pos][words_pos]);
		}
		printf("%s \n", args_values[file_pos + f_pos + 1]);
	}

}

int
main(int argc, char *argv[])
{
	int first_exec_pos = 1;

	if ((argc > MIN_ARGS && is_fmodifier(argv, argc, first_exec_pos) == 1)
	    && is_other_modifier(argv, argc, first_exec_pos) == 0) {
		print_output(argv, argc, first_exec_pos);
		exit(EXIT_SUCCESS);
	} else
		usage(argv[0]);

}
