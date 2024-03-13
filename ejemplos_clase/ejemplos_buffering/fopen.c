#include <stdio.h>
#include <stdlib.h>
#include <err.h>

int
main(int argc, char *argv[])
{
   FILE *fd;

   fd = fopen("/tmp/hola", "w");

   if(fd == NULL){
    err(EXIT_FAILURE, "error");
   }

  

    exit(EXIT_SUCCESS);
}