#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

enum{
    SourceFile,
    DestinationFile,
    EnoughArgs = 3,
    MaxArgs,
    
};

void
usage(){
	
    fprintf(stderr, "usage: copybytes orfile desfile bufsize [bytes copied]\n");
	exit(EXIT_FAILURE);

}

// comprueba que sea un número y devuelve su valor
long
getnumber(char *str){

    int base;
    char *endptr;
    long val;
    base = 10;

    // Para distinguir entre success/failure después de la llamada
    errno = 0;
    val = strtol(str, &endptr, base);
        
    // Se comprueban posibles errores 
    if (errno != 0) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }

    if (endptr == str) {
        errx(EXIT_FAILURE, "No digits were found");
    }
    // Si llegamos hasta aquí strtol() ha sido capaz de parsear un número
    // Ahora es necesario comprobar si la string ha sido un número o no

    if (*endptr != '\0'){    
        errx(EXIT_FAILURE, "is not a complete number");
    }
    return val;
}

/*void 
copybytes(char *src, char dest, long buffsize, long copybytesize){



} */

int
//void
getfd(char *path, int tipefile){

    int isaccessible;
    struct stat sb;
    int fd;
    mode_t mode = 0666; // Permisos de lectura-escritura para el propietario, grupo y otros

    // source file 
    if (tipefile == SourceFile){

        // permite comprobar si tenemos permisos para leer un fichero
        isaccessible = access(path, R_OK);
        if(isaccessible == -1){
            errx(EXIT_FAILURE, "%s does not exit or cannot be read ", path);
        }

        // permite comprobar si es un fichero normal: no un enlace simbólico, etc.
        if (lstat(path, &sb) == -1) {
            perror("lstat");
            exit(EXIT_FAILURE);
        }
        if((sb.st_mode & S_IFMT) != S_IFREG){
            errx(EXIT_FAILURE, "%s is not a regular file", path); 
        }

        // usar open en modo lectura 
        fd = open(path, O_RDONLY);
        if(fd == -1){
            perror("open");
            exit(EXIT_FAILURE);
        }
    }else if (tipefile == DestinationFile){

        // permite comprobar si tenemos permisos para leer un fichero
        /*isaccessible = access(path, W_OK);
        if(isaccessible == -1){
            errx(EXIT_FAILURE, "%s does not exit or cannot be written ", path);
        }

        // permite comprobar si es un fichero normal: no un enlace simbólico, etc.
        if (lstat(path, &sb) == -1) {
            perror("lstat");
            exit(EXIT_FAILURE);
        }
        if((sb.st_mode & S_IFMT) != S_IFREG){
            errx(EXIT_FAILURE, "%s is not a regular file", path); 
        }*/

        // usar open en modo lectura 
        fd = open(path, O_WRONLY|O_CREAT|O_TRUNC, mode);
        if(fd == -1){
            //chmod(path, mode);
            perror("open");
            exit(EXIT_FAILURE);
        }

        /*// permite comprobar si tenemos permisos para leer un fichero
        isaccessible = access(path, W_OK);
        if(isaccessible == -1){
            errx(EXIT_FAILURE, "%s does not exit or cannot be written ", path);
        }

        // permite comprobar si es un fichero normal: no un enlace simbólico, etc.
        if (lstat(path, &sb) == -1) {
            perror("lstat");
            exit(EXIT_FAILURE);
        }
        if((sb.st_mode & S_IFMT) != S_IFREG){
            //errx(EXIT_FAILURE, "%s is not a regular file", path); 
            chmod(path, mode);
        }*/

    }

    return fd;
}

int
main(int argc, char *argv[]){

    argc--; 
    argv++;

    char *srcpath = argv[0];
    char *destpath = argv[1];
    long buffsize;
    long copybytesize;

    int srcfd;
    int destfd;

    switch(argc){

    case EnoughArgs:
        copybytesize = -1;
        break;

    case MaxArgs:
        copybytesize = getnumber(argv[3]);
        if(copybytesize < 0){
            errx(EXIT_FAILURE, "fourth parameter should be positive");
        }
        break;

    default:
        usage();
    }

    // se comprueba que el tamaño del buffer sea un número y > 0
    buffsize = getnumber(argv[2]);
    if(buffsize <= 0){
        errx(EXIT_FAILURE, "third parameter should be bigger than 0");
    }

    if(strcmp(srcpath, "-") == 0){
        srcfd = STDIN_FILENO;
    }else{
        srcfd = getfd(srcpath, SourceFile); 
    } 

    if(strcmp(destpath, "-") == 0){
        destfd = STDOUT_FILENO;
    }else{
        destfd = getfd(destpath, DestinationFile); 
    } 

    //srcfd = getfd(srcpath, SourceFile); 

    fprintf(stderr, "%d \n", srcfd);
    fprintf(stderr, "%d \n", destfd);

    //destfd = getfd(destpath, DestinationFile);
    
    //copybytes(srcfd, destpath, buffsize, copybytesize); 

    // read y write // para el buffer usar malloc y free por si el número es muy grande

    // close los 2 descriptores usados

    // manejo de errores en todo momento.

    close(srcfd);

    exit(EXIT_SUCCESS);
}