
#include <stdio.h>
#include <stdlib.h>

// modificar esta version para añadi estructura de datos 
void 
printngram(){
    int i; 

    printf("[");
    for(i = 0; ; i < n; i++){
        printf("%c", carr[(pos + i) % n])

    }
    printf("]\n");

}

void 
printngrams(FILE *f, char *carr, int n)
{
    int done = 0;
    int pos = 0;

    if(fread(carr, 1,n,f) != n){
        if(ferror(f)){

        }
        return;
    }
    do{
        printngram(carr, n, pos);
        done = fread(carr + pos, 1,1, f) == 0;
        pos = (pos +1) %n; // tengo que repasar los módulos
    }while(!done);

    if(ferror(f)){
        err(EXIT_FAILURE, "can´t read file");
    }

}
int 
main(int argc, char *argv[]){ // Se agregaron los argumentos que faltaban
    

    int gram;
    FILE *f;
    cahr *carr;
    if(argc != 3)

    ngram = atoi(arv[2])

    if(ngram <= 0 || ngram > Maxgram){


    }
    f = fopen(arv[1], "r")

    if(f == NULL)

    carr = (char *)malloc(ngram);
    if(carr == NULL)

    printngrams(f,carr,ngram)

    exit(EXIT_SUCCESS); 
}
