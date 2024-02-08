#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// crear función que ordena palabras

/*void
swap(char *){


}*/ 

void 
ordenastrings(char **strings, int n){

    int i;
    int j;

    //char *swap;

    for (i = 2; i <= n; i++){
        for(j = 1; j < i; j++){

            printf("comparando %s con %s\n", strings[j], strings[i]);

            // hacer algo cuando el resultado de strcmp es > 0 (s1 > s2)
            if(strcmp(strings[j], strings[i]) > 0){

                //swap(strings[i],strings[j]);

                printf("hay que intercambiar %s por %s\n", strings[j], strings[i]);

                

                // intercambio de posiciones
                //swap = strings[i];

                //strings[]
            }
        }
    }

}


int 
main(int argc, char *argv[]){

    int i; 
    int NumArgs;

    NumArgs = argc - 1;

    ordenastrings(argv, NumArgs);
    // el argumento 0 es el propio nombre del programa
    for (i = 1; i < argc; i++)
    {
        // argumentos ordenado
        printf("%s\n", argv[i]);
    }

    exit(EXIT_SUCCESS);


}