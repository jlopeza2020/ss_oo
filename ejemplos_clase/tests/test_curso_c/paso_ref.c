#include <stdlib.h>
#include <stdio.h>


// crear función que ordena palabras

void 
f(char **b)
{
    *b = "holo";
}


int 
main(int argc, char *argv[]){

    char *a;

    a = "hola";

    f(&a);

    printf("%s\n", a);
    // ordenacaracter()
    exit(EXIT_SUCCESS);


}