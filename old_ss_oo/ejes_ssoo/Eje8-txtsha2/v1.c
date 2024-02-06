#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dirent.h>

enum {
	ZERO = 0,
    ONE = 1,
    BUF_SIZE = 8 * 1024,
	READ_END = 0,
	WRITE_END = 1,
};

void
usage(char *program_name)
{
	fprintf(stderr, "usage: %s dir \n", program_name);
	exit(EXIT_FAILURE);
}

int
typefile(char *file)
{
	char *ret;
	char ext = '.';
	int one = 1 , zero = 0;

	ret = strrchr(file, ext);

	if ((ret != NULL && strcmp(ret, ".txt") == 0)) {
        return one;
	}
	return zero;
}

void
process_dir(char *dir)
{
	char *path = malloc((BUF_SIZE) * sizeof(char));
    DIR *d;
    struct dirent *ent;
	int origin_file;
	int fd[2];
	int status;
	int nr;
	char *bufr;
	char *bufw;

    strcpy(path, dir);

    d = opendir(path);
	if (d == NULL) {
		fprintf(stderr, "%s is not a dir\n", path);
        exit(EXIT_FAILURE);
	}

	chdir(path);

	if(pipe(fd) < 0)
        err(EXIT_FAILURE , "cannot make a pipe");
    switch (fork()) {
        case -1:
            err(EXIT_FAILURE , "cannot fork");
        case 0:
            close(fd[WRITE_END]);
            if(dup2(fd[READ_END], 0) < 0) {
                err(EXIT_FAILURE , "dup failed");
            }
            close(fd[READ_END]);
           	execl( "/usr/bin/sha256sum","sha256sum", NULL);
            err(EXIT_FAILURE , "exec failed");

		default: //father

			while ((ent = readdir(d)) != NULL) {
        
        		if (ent->d_name[0] != '.') {
					if (ent->d_type == DT_REG && typefile(ent->d_name) == 1) {
						origin_file = open(ent->d_name, O_RDONLY);
						bufr = malloc((BUF_SIZE) * sizeof(char));
						bufw = malloc((BUF_SIZE) * sizeof(char));

						while ((nr = read(origin_file, bufr, BUF_SIZE)) != 0) {
							if (nr < 0) {
								err(EXIT_FAILURE, "cannot read");
							}
							if (write(fd[WRITE_END], bufw, nr) != nr) {
								err(EXIT_FAILURE, "cannot write");
							}
						}
						close(origin_file);
						free(bufr);	
						free(bufw);
					}
				}
					
			}
		close(fd[READ_END]);
		close(fd[WRITE_END]);
	}
	closedir(d);
	free(path);

	if(wait(&status) < 0) {
			status = EXIT_FAILURE;
		}
    	exit(WEXITSTATUS(status));
}

int
main(int argc, char *argv[])
{
	int parameters;

	parameters = argc - 1;

	if (parameters == ZERO) {
	    process_dir(".");
		exit(EXIT_SUCCESS);
	}else if(parameters == ONE){
        process_dir(argv[1]);
    }else{
        usage(argv[0]);
    }
}