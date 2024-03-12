#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <err.h>
#include <unistd.h>

int
main(int argc, char *argv[]){

    fprintf(stderr, "hello!\n");
    execl("/bin/ls", "myls", "/tmp", NULL);
    // myls es el nombre que le vamos a llamar al comando
    //execl("/bla/ls", "myls", "/tmp", NULL);

    fprintf(stderr, "bye!\n");
    err(EXIT_FAILURE, "exec failed");
//funcion variadica, no tiene un número de argumentos concreto (arbitrario)


// si todo sale correctamente, después de execl sale correctamente 

}