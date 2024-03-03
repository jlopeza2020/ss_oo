#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>

enum{
    EnoughArgs = 3,
    MaxArgs,
};

void
usage(){
	
    fprintf(stderr, "usage: copybytes orfile desfile bufsize [bytes copied]\n");
	exit(EXIT_FAILURE);

}

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
main(int argc, char *argv[]){

    argc--; 
    argv++;

    char *srcpath = argv[0];
    char *destpath = argv[1];
    long buffsize;
    long copybytesize;

    //int srcfd;
    //int destfd;

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

    // se comprueba que el tamaño del buffer sea un número > 0
    buffsize = getnumber(argv[2]);
    if(buffsize <= 0){
        errx(EXIT_FAILURE, "third parameter should be bigger than 0");
    }

    // comprueba el fichero origen 
    // destino 
    
    //getfd(srcpath); // usar access para comprobar si se puede leer ... 
    // stat para saber si ers directorio o fichero  y comprobar open 
    //getfd(destpath);
    
    //copybytes(srcpath, destpath, buffsize, copybytesize); 

    // read y write 

    // close los 2 descriptores usados

    exit(EXIT_SUCCESS);
}