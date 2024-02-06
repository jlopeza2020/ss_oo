#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

enum {
	MIN_ARGS = 2,
	MAX_ARGS = 3,
	BUFSIZE = 10,
};

void
usage(char *program_name)
{
	fprintf(stderr, "usage: %s origin_dir des_dir [amount bytes > 0] \n",
		program_name);
	exit(EXIT_FAILURE);
}

int
is_dir(char **args_values, int amount_args, int first_pos)
{
	int current_pos;
	char slash = '/';
	int count_dir = 0;

	for (current_pos = first_pos; current_pos < amount_args; current_pos++) {

		if (args_values[current_pos][0] == slash) {
			count_dir++;
		}
	}

	return count_dir;
}

int
is_hyphen(char **args_values, int amount_args, int first_pos)
{
	int current_pos;
	char hyphen = '-';
	int count_std = 0;

	for (current_pos = first_pos; current_pos < amount_args; current_pos++) {

		if ((args_values[current_pos][0] == hyphen)
		    && (strlen(args_values[current_pos]) == 1)) {
			count_std++;
		}
	}

	return count_std;
}

long
is_lnguint(char **args_values, int pos)
{
	char *ptr;
	long ret;

	ret = strtol(args_values[pos], &ptr, 10);

	if (ptr[0] != '\0' || ret < 0)
		usage(args_values[0]);

	return ret;
}

int
is_standar_output(char **args_values, int amount_args, int first_pos)
{
	int current_pos;
	char hyphen = '-';
	char slash = '/';
	int count_pos_hyphen;
	int count_pos_dir;
	int is_std_output;

	for (current_pos = first_pos; current_pos < amount_args; current_pos++) {

		if ((args_values[current_pos][0] == hyphen)) {
			count_pos_hyphen = current_pos;
		} else if ((args_values[current_pos][0] == slash)) {
			count_pos_dir = current_pos;
		}
	}
	//check if it is a standar output
	if (count_pos_hyphen > count_pos_dir) {
		is_std_output = 1;
	} else {
		is_std_output = 0;
	}

	return is_std_output;
}

void
cpy_nbytes(long amount_bytes, int of, int ef)
{
	char *buf = malloc((amount_bytes + 1) * sizeof(char));
	int nr;

	nr = read(of, buf, amount_bytes);
	if (nr < 0) {
		err(EXIT_FAILURE, "cannot read");
	}
	if (write(ef, buf, nr) != nr) {
		err(EXIT_FAILURE, "cannot write");
	}
	free(buf);
}

void
cpy_allbytes(int of, int ef)
{
	char *buf = malloc((BUFSIZE + 1) * sizeof(char));
	int nr;

	while ((nr = read(of, buf, BUFSIZE)) != 0) {
		if (nr < 0) {
			err(EXIT_FAILURE, "cannot read");
		}
		if (write(ef, buf, nr) != nr) {
			err(EXIT_FAILURE, "cannot write");
		}
	}
	free(buf);

}

void
cpy_between_files(char **args_values, int parameters, int first_pos)
{
	long amount_bytes;
	int second_pos;
	int origin_file;
	int destination_file;

	second_pos = first_pos + 1;

	origin_file = open(args_values[first_pos], O_RDONLY | O_CREAT, 0666);
	destination_file =
	    open(args_values[second_pos], O_WRONLY | O_CREAT, 0666);

	if (parameters == MIN_ARGS) {
		cpy_allbytes(origin_file, destination_file);
	} else {
		amount_bytes = is_lnguint(args_values, MAX_ARGS);
		cpy_nbytes(amount_bytes, origin_file, destination_file);
	}
	close(origin_file);
	close(destination_file);
}

void
cpy_std2file(char **args_values, int parameters, int first_pos)
{
	long amount_bytes;
	int second_pos;
	int standar_input;
	int destination_file;

	standar_input = 0;
	second_pos = first_pos + 1;

	destination_file =
	    open(args_values[second_pos], O_WRONLY | O_CREAT, 0666);

	if (parameters == MIN_ARGS) {
		cpy_allbytes(standar_input, destination_file);
	} else {
		amount_bytes = is_lnguint(args_values, MAX_ARGS);
		cpy_nbytes(amount_bytes, standar_input, destination_file);
	}
	close(destination_file);
}

void
cpy_file2std(char **args_values, int parameters, int first_pos)
{
	long amount_bytes;
	int origin_file;
	int standar_output;

	standar_output = 1;
	origin_file = open(args_values[first_pos], O_RDONLY | O_CREAT, 0666);

	if (parameters == MIN_ARGS) {
		cpy_allbytes(origin_file, standar_output);
	} else {
		amount_bytes = is_lnguint(args_values, MAX_ARGS);
		cpy_nbytes(amount_bytes, origin_file, standar_output);
	}
	close(origin_file);
}

void
cpy_between_standar(char **args_values, int parameters)
{
	long amount_bytes;
	int standar_input;
	int standar_output;

	standar_input = 0;
	standar_output = 1;

	if (parameters == MIN_ARGS) {
		cpy_allbytes(standar_input, standar_output);
	} else {
		amount_bytes = is_lnguint(args_values, MAX_ARGS);
		cpy_nbytes(amount_bytes, standar_input, standar_output);
	}
}

void
copybytes(char **args_values, int amount_args, int first_pos, int params)
{
	if (is_dir(args_values, amount_args, first_pos) == 2) {
		cpy_between_files(args_values, params, first_pos);

	} else if (is_dir(args_values, amount_args, first_pos) == 1
		   && is_hyphen(args_values, amount_args, first_pos) == 1
		   && is_standar_output(args_values, amount_args,
					first_pos) == 0) {
		cpy_std2file(args_values, params, first_pos);
	} else if (is_dir(args_values, amount_args, first_pos) == 1
		   && is_hyphen(args_values, amount_args, first_pos) == 1
		   && is_standar_output(args_values, amount_args,
					first_pos) == 1) {
		cpy_file2std(args_values, params, first_pos);
	} else if (is_hyphen(args_values, amount_args, first_pos) == 2) {
		cpy_between_standar(args_values, params);
	} else {
		usage(args_values[0]);
	}
}

int
main(int argc, char *argv[])
{
	int first_pos = 1;
	int parameters;

	parameters = argc - 1;

	if ((parameters == MAX_ARGS) || (parameters == MIN_ARGS)) {
		copybytes(argv, argc, first_pos, parameters);
		exit(EXIT_SUCCESS);
	} else {
		usage(argv[0]);
	}

}
