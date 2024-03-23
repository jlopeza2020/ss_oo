#include <stdlib.h>
#include <stdio.h>
#include <err.h>


enum {
	ZeroArgs,
    MaxLine = 256,
};


void
usage(void)
{
	fprintf(stderr, "usage: ./sourcefiles\n");
	exit(EXIT_FAILURE);
}

int 
main(int argc, char *argv[]){

    // preguntar si hace falta malloc
    char line[MaxLine];
    
    argc--;
	argv++;

    if (argc != ZeroArgs) {
		usage();
	}
    

    while(fgets(line, MaxLine, stdin) != NULL){
        printf("line: %s\n", line);

    }
    if(!feof(stdin)){
        errx(EXIT_FAILURE, "eof not reached");
    }

    exit(EXIT_SUCCESS);
}