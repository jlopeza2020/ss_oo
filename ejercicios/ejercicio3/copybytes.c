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
	
    fprintf(stderr, "usage: copybytes orfile desfile buffersize [bytes copied]\n");
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

int
getfd(char *path, int tipefile){

    int isaccessible;
    struct stat sb;
    int fd;
    // Permisos de lectura-escritura para el propietario, grupo y otros
    mode_t mode = 0666;

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

        // usar open en modo escritura y sino crear el fichero 
        // con los permisos escritos en el tercer parámetro 
        fd = open(path, O_WRONLY|O_CREAT|O_TRUNC, mode);
        if(fd == -1){
            //chmod(path, mode);
            perror("open");
            exit(EXIT_FAILURE);
        }
    }

    return fd;
}

void 
copybytes(int srcfd, int destfd, int buffsize, int copybytesize){

    // read y write 
    // para el buffer usar malloc y free por si el número es muy grande
    // incluir lseek para ver si se ha llegado al objetivo deseado

    //ssize t read(int fd, void *buf, size t count);
    // 0  -> final de fichero
    // -1 -> ha habido un error
    //ssize t write(int fd, const void *buf, size t count);
    
    // si write devuelve un número distinto al número de bytes 
    // solicitado en el tercer parámetro, se debe considerar un error. Esto
    // es fácil de entender con un ejemplo de la vida real.

    // tamaño del buffer con malloc

    char *buffer; 
    int nr; 
    //off_t offset;
    //int posfinal;

    //offset = 0;

    buffer = (char *)malloc(sizeof(char)*buffsize); // tamaño de una variable entera * 128 (numero de veces que queremos crear la estructura) 

    if (buffer == NULL) {
        fprintf(stderr, "Error: dynamic memory cannot be allocated\n");
        exit(EXIT_FAILURE);
    }

    // set offset y repasarlo
    while((nr = read(srcfd, buffer, buffsize)) != 0) { 
        fprintf(stderr, "valor de nr es : %d\n", nr);
        if (nr < 0){
            err(EXIT_FAILURE, "can't read");
        }
        // mirar lseek y hacer los cambios
        if(copybytesize > 0){
            // si la posición actual es mayor que la objetivo
            // se ha leído más de lo debido 
            if(nr > copybytesize){
               //posfinal =  nr - copybytesize;
                nr =  nr - copybytesize;
                fprintf(stderr, "valor final - de nr es : %d\n", nr);

                //offset = lseek(srcfd, -nr, SEEK_CUR);
                nr = lseek(srcfd, -nr, SEEK_CUR);


                fprintf(stderr, "el offset - se situa en : %d\n", nr);

            }
            /*else{
                // se ha leido menos de lo debido
                //seguir leyendo 
                nr = nr + copybytesize;
                fprintf(stderr, "valor final  + de nr es : %d\n", nr);

                //offset = lseek(srcfd, -nr, SEEK_CUR);
                nr = lseek(srcfd, nr, SEEK_CUR);

                fprintf(stderr, "el offset +  se situa en : %d\n", nr);

            }*/
        }

        if(write(destfd, buffer, nr) != nr){
            err(EXIT_FAILURE, "can't write");
        }
        //offset += nr;

        
        
        // salir del bucle cuando se haya cumplido el número de 
        // bytes copiados del cuarto parámetro
        if(copybytesize > 0 && nr == copybytesize){
            break;
        }
    }



    //*cadena = "hola buenos dias";

    
    //x = sizeof(int);
    //strcpy(buffer, "hola buenos dias"); // copia de la cadena en el espacio de memoria asignado

    //printf("%s\n", buffer);

    free(buffer);
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

    // se obtienen los descriptores de ficheros de entrada y salida
    // dependiendo de la string obtenida
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

    copybytes(srcfd, destfd, buffsize, copybytesize); 

    // close los 2 descriptores usados
    close(srcfd);
    close(destfd);

    exit(EXIT_SUCCESS);
}