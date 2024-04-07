#include <stdlib.h>
#include <errno.h>
#include <err.h>
#include <stdio.h>

enum {
	TwoArgs = 2,
};
void
usage(void)
{
	fprintf(stderr, "usage: pspids PID0 PID1\n");
	exit(EXIT_FAILURE);
}

long long
getnumber(char *str)
{
	int base;
	char *endptr;
	long val;

	base = 10;

	errno = 0;
	val = strtoll(str, &endptr, base);

	// Se comprueban posibles errores 
	if (errno != 0) {
		err(EXIT_FAILURE, "strtol");
	}
	if (endptr == str) {
		errx(EXIT_FAILURE, "No digits were found");
	}
	// Ahora es necesario comprobar si la string ha sido un número o no
	if (*endptr != '\0') {
		errx(EXIT_FAILURE, "is not a complete number");
	}
	return val;
}

int 
main(int argc, char *argv[]){
    
    long long pid0;
    long long pid1;
    
    argc--;
	argv++;

    if (argc != TwoArgs) {
		usage();
	}
    pid0 = getnumber(argv[0]);
    pid1 = getnumber(argv[1]);

    if(pid0 <= 0 || pid1 <= 0){
        fprintf(stderr, "error: PIDs should be positive numbers\n");
        usage();
    }
    if(pid1 < pid0){
        fprintf(stderr, "error: PID1 should be greater or equal than PID0\n");
        usage();
    }

    // ejecutar el programa 
    // usar: fork, exec, pipe...

    exit(EXIT_SUCCESS);
}