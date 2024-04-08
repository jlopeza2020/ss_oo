#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

enum {
    Bsize = 1024,
};

void child(int *fd) {
    char buf[Bsize];
    long n;

    // como lo que quiere hacer el hijo es leer del padre, 
    // es necesario  cerrar la parte de escritura del pipe
    close(fd[1]); 

    while ((n = read(fd[0], buf, Bsize)) > 0) {
        buf[n] = '\0';
        printf("Child: %s\n", buf);
    }

    if (n < 0)
        err(EXIT_FAILURE, "cannot read from pipe");

    // como ya ha terminado la parte de la lectura, se cierra el descriptor de fichero
    close(fd[0]);
}

// el padre manda al hijo una cadena de caracteres a través del pipe
int main(int argc, char *argv[]) {
    int fd[2];
    char *str = "I am your father";

    if (pipe(fd) < 0) // se crea antes que el hijo porque sino no lo hereda 
        err(EXIT_FAILURE, "cannot make a pipe");

    switch (fork()) { // crea el proceso hijo
        case -1:
            err(EXIT_FAILURE, "cannot fork");
        case 0:

            child(fd);
            exit(EXIT_SUCCESS);
        default:
            // como el padre lo que quiere hacer es escribir 
            // cierra la parte de la lectura del pipe 
            close(fd[0]);
            if (write(fd[1], str, strlen(str)) != (ssize_t)strlen(str))
                err(EXIT_FAILURE, "error writing in pipe");
            // después de usarla, se cierra la parte de escritura
            close(fd[1]);
    }

    exit(EXIT_SUCCESS);
}
