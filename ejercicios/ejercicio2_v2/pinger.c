#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <err.h>

enum {

	NoArgs,
};

void
usage()
{
	fprintf(stderr, "usage: pinger ip/name [ip/name ...]\n");
	exit(EXIT_FAILURE);
}

pid_t
ping(char *ip)
{

	pid_t pid;

	pid = fork();

	switch (pid) {
	case -1:
		err(EXIT_FAILURE, "fork failed!");
	case 0:
		execl("/bin/ping", "myping", "-c", "1", "-w", "5", ip, NULL);
		err(EXIT_FAILURE, "exec failed");
	default:
	}
	return pid;
}

int
waitall(pid_t pid)
{

	int sts;
	int exitsts;

	exitsts = 0;

	while ((pid = wait(&sts)) != -1) {

		if (WIFEXITED(sts)) {
			if (WEXITSTATUS(sts) != 0) {
				exitsts = 1;
			}
		}
	}

	if (exitsts == 1) {
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int
main(int argc, char *argv[])
{

	int i;
	int status;
	int pid;

	argc--;
	argv++;

	if (argc == NoArgs) {
		usage();
	}

	for (i = 0; i < argc; i++) {
		pid = ping(argv[i]);
	}

	status = waitall(pid);

	exit(status);
}
