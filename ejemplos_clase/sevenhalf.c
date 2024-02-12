#include <stdlib.h>
#include <stdio.h>


enum { 

    MaxScore = 75, // usamos 75 en vez de 7.5 así nos evitamos floats
    HonorVal = 5,
};

int
count(Card *c)
{
    if(ishonor(c) && number(c) != Ace){
        return HonorVal;
    }
    return number(c) * 10;

}

void
usage(void)
{
    fprintf(stderr, "usage: sevenhalf number suit [number suit]... \n");
    exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{

    int i; 

    int val = 0; 

    Card *c; // puntero a una carta


    if( argc % 2 != 1 || argc == 1 ){
        usage();
    }

    // idioms de C (usarlo)
    argc--; // finalmente me quedo con el número real de argumentos
    argv++; // salto a la posición del primer argumento

    for(i = 0; i < argc && val <= MaxScore; i += 2){

        c = newcard(argv[i], argv[i+1]);

        if(c == NULL){
            fprintf(stderr, "fatal error: invalid card\n");
            exit(EXIT_FAILURE);
        }

        printf("card: ");
        printcard(c);
        printf("\n");
        val += count(c);
        free(c);
    }

    if (val > MaxScore) { 
        printf("You lost\n");
        exit(EXIT_FAILURE);
    } else if (val == MaxScore){
        printf("You win! \n");
    }else{ 

        printf("Final score: %f\n", val / 10.0); // entero / float = float 
    }

    
    // BRIEFING
    // mirar si el número de argumentos pinta bien o no 
	// si el número de argumentos es par o uno, malo

    // opciones: ganar (status exito), perder(status failure), o no has llegado
    
    // parará cuando llegue al final de los argumentos  
    // o si nos pasamos de puntuación

	exit(EXIT_SUCCESS);
}