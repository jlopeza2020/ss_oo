#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

enum {
	MAX_ARGS = 1,
	BUF_SIZE = 8 * 1024,
};

void
usage(char *program_name)
{
	fprintf(stderr, "usage: %s dir \n", program_name);
	exit(EXIT_FAILURE);
}

int
isdir(struct stat *st)
{
	int isf;

	isf = (st->st_mode & S_IFMT) == S_IFDIR;
	return isf;
}

int
zcount(int of)
{
	char *buf = malloc((BUF_SIZE) * sizeof(char));
	int nr;
	int i;
	int count;

	count = 0;

	while ((nr = read(of, buf, BUF_SIZE)) != 0) {
		if (nr < 0) {
			err(EXIT_FAILURE, "cannot read");
		}
		for (i = 0; i < nr; i++) {
			if (buf[i] == 0) {
				count++;
			}

		}
	}
	free(buf);
	return count;
}

int
get_int_length(int value)
{
	int length = 1;

	while (value > 9) {
		length++;
		value /= 10;
	}
	return length;
}

void
writefile(int desf, int num_zrs, char *regular_file)
{
	int name_length;
	int zero_length;
	int total_length;
	int i;
	int j;
	int k;

	name_length = strlen(regular_file);
	zero_length = get_int_length(num_zrs);

	char *charValue = malloc((zero_length + 1) * sizeof(char));

	sprintf(charValue, "%d", num_zrs);

	total_length = name_length + zero_length + 2;
	char *buf = malloc((total_length) * sizeof(char));

	k = 0;
	for (i = 0; i < zero_length; i++) {
		buf[k] = charValue[i];
		k++;
	}
	buf[k] = ' ';
	k++;
	for (j = 0; j < name_length; j++) {
		buf[k] = regular_file[j];
		k++;
	}
	buf[k] = '\n';

	if (write(desf, buf, total_length) != total_length) {
		err(EXIT_FAILURE, "cannot write");
	}
	free(buf);
	free(charValue);

}

void
managedir(char **args_values, int amount_args, int first_pos)
{
	DIR *d;
	struct dirent *ent;
	int origin_file;
	int destination_file;
	struct stat st;
	int number_zrs;

	d = opendir(args_values[first_pos]);
	chdir(args_values[first_pos]);

	destination_file = open("z.txt", O_WRONLY | O_TRUNC | O_CREAT, 0666);

	if (d == NULL) {
		err(EXIT_FAILURE, "opendir failed : %s ",
		    args_values[first_pos]);
	}

	while ((ent = readdir(d)) != NULL) {
		stat(ent->d_name, &st);

		if (ent->d_name[0] != '.' && isdir(&st) <= 0
		    && (strcmp(ent->d_name, "z.txt") != 0)) {
			origin_file = open(ent->d_name, O_RDONLY);
			number_zrs = zcount(origin_file);
			writefile(destination_file, number_zrs, ent->d_name);
			close(origin_file);
		}
	}
	close(destination_file);
	closedir(d);
}

int
main(int argc, char *argv[])
{
	int parameters;
	struct stat st;

	parameters = argc - 1;

	if (stat(argv[1], &st) < 0) {
		usage(argv[0]);
	}
	if (parameters == MAX_ARGS && isdir(&st)) {
		managedir(argv, argc, parameters);
		exit(EXIT_SUCCESS);
	} else {
		usage(argv[0]);
	}

}
