#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int 
main(int argc, char *argv[]){

    char *cadena; 

    //int x;

    cadena = (char *)malloc(sizeof(char)*128); // tamaño de una variable entera * 128 (numero de veces que queremos crear la estructura) 

    if (cadena == NULL) {
        fprintf(stderr, "Error: No se pudo asignar memoria\n");
        exit(EXIT_FAILURE);
    }

    //*cadena = "hola buenos dias";

    
    //x = sizeof(int);
    strcpy(cadena, "hola buenos dias"); // copia de la cadena en el espacio de memoria asignado

    printf("%s\n", cadena);

    free(cadena);
    
    

    exit(EXIT_SUCCESS);
}	