#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int nbla;

enum {

	NoArgs,		
};

void
usage(void)
{
	fprintf(stderr,
		"usage: pinger ip/name [ip/name ...]\n");
	exit(EXIT_FAILURE);
}


int
main(int argc, char *argv[])
{
	// Omitimos primer argumento
	argc--;
	argv++;

    // tokenizar argumentos

    if(argc == NoArgs){
        usage();
    }


    pid_t p;
    int i;

    nbla++;

    for (i = 0; i < 2; i++){ 
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

    

    //makeping(argv,argc);
    
	/*forward = "forward";
	backward = "backward";

	sortargs(argv, argc);

	varenv = getenv("DIRECTION");

	if (varenv == NULL || strcmp(varenv, forward) == 0) {
		printforwardargs(argv, argc);

	} else if (strcmp(varenv, backward) == 0) {
		printbackwardargs(argv, argc);

	} else {
		usage();
	}*/
	
	exit(EXIT_SUCCESS);
}
