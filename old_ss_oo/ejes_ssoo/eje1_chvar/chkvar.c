#include <err.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

enum {
	MIN_ARGS = 3,
};

void
usage()
{
	errx(EXIT_FAILURE,
	     "usage: ch varname varcontent [varname varcontent] ...");
}

void
print_middle_errors(char **args_values, int current_pos, int following_pos)
{
	printf(" %s != %s, ", args_values[current_pos],
	       args_values[following_pos]);
}

void
print_last_error(char **args_values, int current_pos, int following_pos)
{
	printf(" %s != %s  ", args_values[current_pos],
	       args_values[following_pos]);
}

void
print_errors(int amount_args, char **args_values, int *array_errs, int errors,
	     int first_pos)
{
	printf("error: ");

	for (int current_pos = first_pos; current_pos < amount_args - 1;
	     current_pos += 2) {
		int following_pos = current_pos + 1;

		if (array_errs[current_pos] == 1) {
			if (errors > 1)
				print_middle_errors(args_values, current_pos,
						    following_pos);
			else
				print_last_error(args_values, current_pos,
						 following_pos);

			errors--;

		}

	}
	printf("\n");
}

int
is_args_correct(int amount_args, char **args_values, int *array_errs,
		int first_pos)
{
	int err_times = 0;

	for (int current_pos = first_pos; current_pos < amount_args - 1;
	     current_pos += 2) {
		int following_pos = current_pos + 1;

		char *s = getenv(args_values[current_pos]);

		if (s != NULL) {
			if (strcmp(s, args_values[following_pos]) != 0) {
				err_times++;
				array_errs[current_pos] = 1;
			}
		} else {
			err_times++;
			array_errs[current_pos] = 1;
		}
	}
	return err_times;
}

void
print_output(int amount_args, char **args_values, int first_pos)
{
	int *array_errors = malloc((amount_args - 1) * sizeof(int));
	int count_errors = 0;

	count_errors =
	    is_args_correct(amount_args, args_values, array_errors, first_pos);
	if (count_errors != 0) {
		print_errors(amount_args, args_values, array_errors,
			     count_errors, first_pos);
		free(array_errors);
		exit(EXIT_FAILURE);
	} else {
		free(array_errors);
		exit(EXIT_SUCCESS);
	}

}

int
main(int argc, char *argv[])
{
	//Is bigger or equals to 3 because in argv the first value
	// is when the function calls itself; so, the total number of
	// arguments must be odd. (even +1 = odd) 
	int first_arg_pos = 1;

	if (argc >= MIN_ARGS && argc % 2 != 0)
		print_output(argc, argv, first_arg_pos);
	else
		usage();
}
