#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>

int
main(int argc, char *argv[]){
    pid_t id = fork();
    int n;
    int sts;

    // hijo
    if(id == 0){ 
        n = 1;
    }else{ // padre

        n = 6;
    }
    // al padre sólo le da tiempo asignarse el valor de 6
    // y espera a que su hijo acabe 
    if(id != 0){
        wait(&sts);
    }
    int i; 

    for(i = n; i < n + 5; i++){
        printf("%d ", i);
    }
    printf("\n");

    exit(EXIT_SUCCESS);
}