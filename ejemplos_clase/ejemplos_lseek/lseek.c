#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>



int 
main(int argc, char *argv[]){


    int f;
    char buf[6];
    off_t offset;
    f=open("seek", O_RDWR);
    // lee y escribe los 6 primeros caracteres
    read(f,buf,6); 
    write(1,buf,6);
    // saltas los 5 siguientes 
    offset = lseek(f,5,SEEK_CUR);
    printf("%ld\n", offset);
    // a partir de esos 5 saltados, vuelves a leer y escribir
    read(f,buf,6);
    write(1,buf,6);

    exit(EXIT_SUCCESS);
}