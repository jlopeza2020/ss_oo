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
	ZERO = 0,
	BUF_SIZE = 8 * 1024,
};

void
usage(char *program_name)
{
	fprintf(stderr, "usage: %s \n", program_name);
	exit(EXIT_FAILURE);
}

void
process_file(char *filename, int *totalreadbytes)
{
	unsigned char *bufer = malloc((BUF_SIZE) * sizeof(unsigned char));
	FILE *file;

	file = fopen(filename, "r");
	if (!file) {
		fprintf(stderr, "Cannot open file %s\n", filename);
	}
	while (!feof(file)) {
		*totalreadbytes =
		    fread(bufer, sizeof(char), sizeof(bufer),
			  file) + *totalreadbytes;
	}

	free(bufer);
	fclose(file);
}

void
typefile(char *file, int *numcfiles, int *numhfiles, int *total_bytes)
{
	char *ret;
	char ext = '.';

	ret = strrchr(file, ext);

	if ((ret != NULL && strcmp(ret, ".c") == 0)) {
		*numcfiles = *numcfiles + 1;
		process_file(file, total_bytes);

	} else if ((ret != NULL && strcmp(ret, ".h") == 0)) {
		*numhfiles = *numhfiles + 1;
		process_file(file, total_bytes);
	}
}

void
process_path(char *path, int *numcfiles, int *numhfiles, int *total_bytes)
{
	DIR *d;
	struct dirent *ent;

	int length_name;
	int length_path;
	int total_length;

	length_path = strlen(path);

	d = opendir(path);
	if (d == NULL) {

		fprintf(stderr, "opendir failed: %s \n", path);

	}

	while ((ent = readdir(d)) != NULL) {

		if (ent->d_name[0] != '.') {

			length_name = strlen(ent->d_name);
			total_length = length_path + length_name;
			//2 because of '/' and '0'
			char *new_path =
			    malloc((total_length + 2) * sizeof(char));

			char tmpchain[1];

			tmpchain[0] = '/';
			strcat(strcpy(new_path, path), tmpchain);
			strcat(new_path, ent->d_name);

			if (ent->d_type == DT_DIR) {
				process_path(new_path, numcfiles, numhfiles,
					     total_bytes);

			} else if (ent->d_type == DT_REG) {
				typefile(new_path, numcfiles, numhfiles,
					 total_bytes);
			}
			free(new_path);
		}
	}
	closedir(d);
}

void
process_stdin()
{
	char *path = malloc((BUF_SIZE) * sizeof(char));
	struct stat st;
	char *real_path;

	int number_cfiles;
	int number_hfiles;
	int total_bytes;

	while (fgets(path, BUF_SIZE, stdin) != NULL) {
		real_path = strtok(path, "\n");

		if (stat(real_path, &st) < 0) {
			fprintf(stderr, "%s is not a path\n", real_path);
		} else {

			number_cfiles = 0;
			number_hfiles = 0;
			total_bytes = 0;

			process_path(real_path, &number_cfiles, &number_hfiles,
				     &total_bytes);
			printf("%s\t%d\t%d\t%d\n", real_path, number_cfiles,
			       number_hfiles, total_bytes);
		}
	}
	free(path);
}

int
main(int argc, char *argv[])
{
	int parameters;

	parameters = argc - 1;

	if (parameters == ZERO) {
		process_stdin();
		exit(EXIT_SUCCESS);
	} else {
		usage(argv[0]);
	}
}
