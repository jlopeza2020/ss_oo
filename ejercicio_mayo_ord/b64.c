#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

enum{
    READ,
    WRITE,
    MaxBuffer = 4*1024,
};

void
usage(void){
    fprintf(stderr, "Usage: b64 file offset\n");
    exit(EXIT_FAILURE);
}

long
getnumber(char *str){

    char *endptr;
    long val;

    // se inicializa para ver si hay algún error
    errno = 0;  
    val = strtol(str, &endptr, 10);

    // mira si existen errores
    if (errno != 0) {
        return -1;
    }

    if (endptr == str) {
        return -1;
    }

    if (*endptr != '\0')
        return -1;

    return val;
}

int
main(int argc, char *argv[]){

    int mypipe[2];
    char buffer[MaxBuffer];
    int fd;
    int offset;
    FILE *fdo;
    argc--;
    argv++;

    if(argc != 2){
        usage();
    }

    fd = open(argv[0], O_RDONLY);
    if(fd == -1){
        err(EXIT_FAILURE, "File does not exist\n");
    }
    //offset = atoi(argv[1]);
    offset = getnumber(argv[1]);
    if(offset == -1){
        close(fd);
        errx(EXIT_FAILURE, "Error: second parameter failed\n");
    }
    // se mueve el offset hasta donde se elige con el segundo parámetro
    lseek(fd,offset, SEEK_SET);

    if(pipe(mypipe) == -1){
        err(EXIT_FAILURE, "Error: pipe failed\n");
    }
    if(dup2(fd, 0) == -1){
        err(EXIT_FAILURE, "Error: dup failed\n");
    }
    close(fd);
    // hacer el fork
    switch(fork()){
        case -1: 
            break;
        case 0: 
            close(mypipe[READ]);
            if(dup2(mypipe[WRITE], 1) == -1){
                err(EXIT_FAILURE, "Error: dup failed\n");
            }
            close(mypipe[WRITE]);
            execl("/bin/base64", "base64", NULL);
            err(EXIT_FAILURE, "Error: execl failed\n");
            break;
        default:
            close(mypipe[WRITE]);
            if(dup2(mypipe[READ], 0) == -1){
                err(EXIT_FAILURE, "Error: dup failed\n");
            }
            // escribe
            fdo = fdopen(mypipe[READ], "r");
            while(fgets(buffer, MaxBuffer, fdo) != NULL){
                // imprime desde el offset
                printf("%s", buffer);
            }
            fclose(fdo);
            close(mypipe[READ]);
    }

    exit(EXIT_SUCCESS);
}