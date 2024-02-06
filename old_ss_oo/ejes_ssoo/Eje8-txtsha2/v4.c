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

	ret = strrchr(file, ext);

	if ((ret != NULL && strcmp(ret, ".txt") == 0)) {
        return ONE;
	}
	return ZERO;
}
/*
void
close_pipes(int fd_read, int fd_write, int fd2_read, int fd2_write)
{
	close(fd_read);
	close(fd_write);
	close(fd2_read);
	close(fd2_write);

}
*/
void
execute_sha(int fd_read, int fd_write, int fd2_read, int fd2_write)
{
	close(fd_write);
    if(dup2(fd_read, STDIN_FILENO) < 0) {

		close(fd_read);
		close(fd2_read);
		close(fd2_write);
    	err(EXIT_FAILURE , "dup failed");
    }
    close(fd_read);

	if(dup2(fd2_write, STDOUT_FILENO) < 0) {

		close(fd2_read);
		close(fd2_write);
        err(EXIT_FAILURE , "dup failed");
    }

	close(fd2_write);
    execl( "/usr/bin/sha256sum","sha256sum", NULL);
	close(fd2_read);
    err(EXIT_FAILURE , "exec failed");
}

void 
process_txtfiles(DIR *d, int fd_read, int fd_write, int fd2_read, int fd2_write)
{
	struct dirent *ent;
	int origin_file, nr;
	char *buffer;

	while ((ent = readdir(d)) != NULL) {
		
        if (ent->d_name[0] != '.') {
			if (ent->d_type == DT_REG && typefile(ent->d_name) == 1) {
				origin_file = open(ent->d_name, O_RDONLY);
				buffer = malloc((BUF_SIZE) * sizeof(char));

				while ((nr = read(origin_file, buffer, BUF_SIZE)) != 0) {
					if (nr < 0) {
						close(fd2_read);
						close(fd_write);
						close(fd2_write);
						err(EXIT_FAILURE, "cannot read");
					}
					if (write(fd_write, buffer, nr) != nr) {
						close(fd2_read);
						close(fd_write);
						close(fd2_write);
						err(EXIT_FAILURE, "cannot write");
					}
				}
				close(origin_file);
				free(buffer);	
			}
		}			
	}
	closedir(d);
}

void
process_dir(char *dir)
{
    DIR *d;
	int fd[2], fd2[2], status, nr;
	char *buf;

    d = opendir(dir);
	if (d == NULL) {
		fprintf(stderr, "%s is not a dir\n", dir);
        exit(EXIT_FAILURE);
	}

	chdir(dir);

	if(pipe(fd) < 0 || pipe(fd2) < 0)
	{
		close(fd[READ_END]);
		close(fd2[READ_END]);
		close(fd[WRITE_END]);
		close(fd2[WRITE_END]);
        err(EXIT_FAILURE , "cannot make a pipe");
	}
    switch (fork()) {
        case -1:
			close(fd[READ_END]);
			close(fd2[READ_END]);
			close(fd[WRITE_END]);
			close(fd2[WRITE_END]);
            err(EXIT_FAILURE , "cannot fork");
        case 0:
			execute_sha(fd[READ_END],fd[WRITE_END],fd2[READ_END],fd2[WRITE_END]);
		default: //father
			close(fd[READ_END]);
			process_txtfiles(d,fd[READ_END],fd[WRITE_END],fd2[READ_END],fd2[WRITE_END]);
		close(fd[WRITE_END]);
		close(fd2[WRITE_END]);
		buf = malloc((BUF_SIZE) * sizeof(char));

		while ((nr = read(fd2[READ_END], buf, BUF_SIZE)) != 0) {
			if (nr < 0) {
				close(fd2[READ_END]);
				err(EXIT_FAILURE, "cannot read");
			}
			strtok(buf,"	-");			
			if (write(STDOUT_FILENO,buf,nr) != nr) {
				close(fd2[READ_END]);
				err(EXIT_FAILURE, "cannot write");
			}
		}
		free(buf);	
	}

	if(wait(&status) < 0)
		status = EXIT_FAILURE;
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