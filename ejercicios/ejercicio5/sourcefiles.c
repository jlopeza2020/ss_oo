#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <sys/types.h>
#include <dirent.h>



enum {
	ZeroArgs,
    MaxLine = 256,
};

struct Sourcefiles{
    char *path;
    long cfiles;
    long hfiles;
    long long totalbytes;
};

typedef struct Sourcefiles Sourcefiles;

void
usage(void)
{
	fprintf(stderr, "usage: ./sourcefiles\n");
	exit(EXIT_FAILURE);
}

int 
main(int argc, char *argv[]){

    // preguntar si hace falta malloc
    /*char line[MaxLine];
    
    argc--;
	argv++;

    if (argc != ZeroArgs) {
		usage();
	}*/

    
    DIR *d;
    struct dirent *ent;

    if (argc != 2) {
        errx(EXIT_FAILURE, "Uso: %s dir", argv[0]);
    }

    d = opendir(argv[1]);
    if (d == NULL) {
        err(EXIT_FAILURE, "opendir failed: %s", argv[1]);
    }

    while ((ent = readdir(d)) != NULL) {
        if (ent->d_name[0] != '.') {
            printf("%s\n", ent->d_name);
        }
    }

    closedir(d);




    /*while(fgets(line, MaxLine, stdin) != NULL){
        printf("line: %s\n", line);


    // se crea una estructura de datos
    // cada vez que se accede a una entrada de directorio

    

    }
    if(!feof(stdin)){
        errx(EXIT_FAILURE, "eof not reached");
    }*/


    exit(EXIT_SUCCESS);
}