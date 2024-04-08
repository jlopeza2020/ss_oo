#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int fd[2];
    char *str = "I am your father\n";
    int status;

    // hacerlo antes de fork para que lo herede el hijo
    if (pipe(fd) < 0) {
        err(EXIT_FAILURE, "cannot make a pipe");
    }

    switch (fork()) {
        case -1:
            err(EXIT_FAILURE, "fork failed");
        case 0:
           // el hijo como quiere leer, cierra el descriptor de escritura
            close(fd[1]);
            // ponemos el descriptor de lectura como entrada estándar
            if (dup2(fd[0], 0) < 0) {
                err(EXIT_FAILURE, "dup failed");
            }
            // cerramos el descriptor de lectura ya que ya lo hemos usado
            close(fd[0]);
            // ejecutamos el programa
            execl("/usr/bin/wc", "wc", "-w", NULL);
            err(EXIT_FAILURE, "exec failed");
        default:
            // cerramos el descriptor de lectura
            close(fd[0]);
            // escribimos en el descriptor de escritura  "I am your father"
            if (write(fd[1], str, strlen(str)) != (ssize_t)strlen(str)) {
                err(EXIT_FAILURE, "error writing in pipe");
            }
            // ya hemos terminado de usar: cerramos el descriptor de escritura
            close(fd[1]);
            // esperamos a que acabe el hijo
            if (wait(&status) < 0) {
                status = EXIT_FAILURE;
            }
    }

    exit(status);
}
