#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int nbla;

int
main(int argc, char *argv[]){

    pid_t p;

    nbla++;

    p = fork();
    if(p == 0){
        nbla++;
        printf("soy el hijo %d nbla:%d\n", p,nbla);
        exit(EXIT_SUCCESS);
    }else{
        nbla++;
        printf("soy el padre %d nbla:%d\n", p, nbla);


    }
    printf("sigo siendo el padre %d nbla: %d\n", p, nbla);
}