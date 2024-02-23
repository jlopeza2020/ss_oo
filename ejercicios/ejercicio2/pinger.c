#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
enum {

	NoArgs,
};

void
usage(void)
{
	fprintf(stderr, "usage: pinger ip/name [ip/name ...]\n");
	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	int pid;
	int sts;
	int i;
	int exitsts;

	// Omitimos primer argumento
	argc--;
	argv++;

	if (argc == NoArgs) {
		usage();
	}

	for (i = 0; i < argc; i++) {

		pid = fork();

		switch (pid) {
		case -1:
			err(EXIT_FAILURE, "fork failed!");
		case 0:
			execl("/bin/ping", "ping", "-c", "1", "-w", "5",
			      argv[i], NULL);
			err(EXIT_FAILURE, "exec failed");
		default:
			//printf("child created : %d\n", pid );
		}
	}

	exitsts = 0;
	while ((pid = wait(&sts)) != -1) {

		if (WIFEXITED(sts)) {
			if (WEXITSTATUS(sts) != 0) {
				exitsts = 1;
			}
		}
	}

	if (exitsts == 1) {
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
