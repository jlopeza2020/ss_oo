#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parser.h"

enum {
	ZeroArgs,
	MaxLine = 512,
};

void
usage(void)
{
	fprintf(stderr, "usage: ./shell\n");
	exit(EXIT_FAILURE);
}


int 
main(int argc, char *argv[]){

    char line[MaxLine];
    int numwords;
    CheckInput checkinput;

    argc--;
	argv++;

    if (argc != ZeroArgs) {
		usage();
	}

    printf("Shell made by Julia López Augusto\n");
	printf("Write EXIT to leave\n");


    while (1) {

        if(fgets(line, MaxLine, stdin) == NULL){
            exit(EXIT_SUCCESS);
        }
        /*if (feof(stdin)) {
            // cerrar todo lo necesario y salir
            freememory(&checkinput, numwords);
            exit(EXIT_SUCCESS);
        }*/

        // elimina el salto de línea al final
		line[strcspn(line, "\n")] = 0;

        numwords = getnumwords(line);
        tokenize(&checkinput, line, numwords);

        //printf("%d\n", numwords);

        // ya sabemos el número de palabras que 
        // tiene nuestra linea de la terminal
        freememory(&checkinput, numwords);

        if(strcmp(line, "EXIT") == 0){
            // cerrar todo lo necesario y salir
            //freememory(&checkinput, numwords);
            exit(EXIT_SUCCESS);
        }
    }

    //freememory(&checkinput, numwords);
    exit(EXIT_SUCCESS);
}