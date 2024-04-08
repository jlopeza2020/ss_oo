#include <stdlib.h>
#include <errno.h>
#include <err.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


enum {
	TwoArgs = 2,
	MaxLine = 4*1024,
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

// lo que hace el hijo es ejecutar ps y escribir su salida 
// en vez por la estandar, por la entrada (fd[1]) del pipe
void 
executeps(int *fd){

	// cerramos el extremo de lectura
	close(fd[0]);

	// escribir el contenido de la ejecución 
	// en la entrada de escritura del pipe
	dup2(fd[1], 1);
	close(fd[1]);

	execl("/bin/ps", "ps","-eo", "%p %U %x %c", NULL);
	err(EXIT_FAILURE, "exec failed");
}

void
printlines(int *fd, long long pid0, long long pid1){

	char line[MaxLine];
	FILE *pipe_read;

	// fgets lee de la entrada estándar por eso
	// hay que cerrar el descriptor de escritura
	close(fd[1]);
	
	// hay que revisar lo de crear fichero
	pipe_read = fdopen(fd[0], "r");
    if (pipe_read == NULL) {
        perror("fdopen");

    }
    while (fgets(line, MaxLine, pipe_read) != NULL) {
        printf("%s", line);
		// tratar si la línea es mayor de la que tengo
		// hacer el parsing correspondiente 
    }

	fclose(pipe_read);
	close(fd[0]);

}

int 
main(int argc, char *argv[]){
    
    long long pid0;
    long long pid1;
	int fd[2]; // fd[0] es de lectura y fd[1] es de escritura
    int status;

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

	// se crea antes que el fork para que lo herede el proceso hijo
	if(pipe(fd) < 0){
		err(EXIT_FAILURE, "cannot make a pipe");
	}

	switch (fork()) { // crea el proceso hijo
        case -1:
            err(EXIT_FAILURE, "cannot fork");
        case 0:
            executeps(fd);
            exit(EXIT_SUCCESS);
        default:
			printlines(fd, pid0, pid1);
			// esperamos a que acabe el hijo
            if (wait(&status) < 0) {
                status = EXIT_FAILURE;
            }
    }
	
	

    exit(EXIT_SUCCESS);
}