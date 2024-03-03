#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

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
isnumber(char *str){

    int base;
    char *endptr;
    long val;
    base = 10;

    // Para distinguir entre success/failure después de la llamada
    errno = 0;
    val = strtol(str, &endptr, base);
        
    //strtol(str, &endptr, base);


    // Se comprueban posibles errores 
    if (errno != 0) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }

    if (endptr == str) {
        fprintf(stderr, "No digits were found\n");
        exit(EXIT_FAILURE);
    }
    // Si llegamos hasta aquí strtol() ha sido capaz de parsear un número
    // Ahora es necesario comprobar si la string ha sido un número o no

    if (*endptr != '\0'){    
        // Devuelve fallo
        //return 0;
        fprintf(stderr, "Is not a complete number\n");
        exit(EXIT_FAILURE);
    }
    // Devuelve éxito
    return val;
}

int
main(int argc, char *argv[]){

    long buffsize;
    
    argc--; 
    argv++;

    switch(argc){

    case EnoughArgs:
        // se comprueba que el tamaño del buffer sea un número
        buffsize = isnumber(argv[2]);
        printf("es un numero: %ld\n", buffsize);

        break;
    case MaxArgs:
        // se comprueba que el tamaño del buffer sea un número
        buffsize = isnumber(argv[2]);
        printf("es un numero: %ld\n", buffsize);

        
        break;

    default:
        usage();
    }

    printf("hola\n");

    // comprueba el fichero origen 
    // destino 
    // se opera 


    /*
    // comprueba el número de argumentos
    if (argc < Enough || argc > Maximun){
        usage();
    }
    // 
    if ()

    // comprueba si el tercer y cuarto parámetro es un entero
    // strtol y casting 
    // comprobar que el buffer sea >= 0
    // comprobar que el numero de bytes copiados sea > 0
    if(!isnumber()){

    }*/

    // usar err





    
    exit(EXIT_SUCCESS);
}