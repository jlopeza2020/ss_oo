#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

enum{
    EnoughArgs = 3,
    MaxArgs,
};

void
usage(){
	
    fprintf(stderr, "usage: copybytes orfile desfile bufsize [bytes copied]\n");
	exit(EXIT_FAILURE);

}

int
isnumber(){


}

int
main(int argc, char *argv[]){

    //char *buffer; 
    //char *numbytes;
    
    argc--; 
    argv++;

    switch(argc){

    case EnoughArgs:
            // trata solo lo de 3 argumentos 
        //printf("hola 3 \n");
        if (isnumber())
        break;
    case MaxArgs:
            // trata 4 argumentos
        //printf("hola 4 \n");
        break;

    default:
        usage();
    }


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