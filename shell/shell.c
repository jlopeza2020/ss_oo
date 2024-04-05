#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parser.h"

enum {
	ZeroArgs,
	MaxLine = 512, // más grande  // manejar el error si la string es más grande : 2k  
};

void
usage(void)
{
	fprintf(stderr, "usage: ./shell\n");
	exit(EXIT_FAILURE);
}

void
initcl(CommandLine *cl){

    cl->numwords = 0;
    cl->bg = 0;
    cl->stdired = 0;
    cl->stdored = 0;
	cl->numpipes = 0;
    cl->env = 0;
    cl->equal = 0;
}

int
main(int argc, char *argv[])
{

	char line[MaxLine];

	//int numwords;
	CommandLine cl;
	// inicializar la estructura
	initcl(&cl);

	argc--;
	argv++;

	if (argc != ZeroArgs) {
		usage();
	}

	printf("Shell made by Julia López Augusto\n");
	printf("Write EXIT to leave\n");

	while (1) {

		if (fgets(line, MaxLine, stdin) == NULL) {
			// break  // comprobar a final si ha llegado con error o final de fichero 
			exit(EXIT_SUCCESS);
		}

		// mirar si el tamaño de fgets se excede del tamaño 
		// HAY UE MODIFICARLO
		// elimina el salto de línea al final
		line[strcspn(line, "\n")] = 0;  // pone el último byte \0 es como strchr 

		cl.numwords = getnumwords(line);
		tokenize(&cl, line);
		// una vez tokenizado hay que distinguir cada caso
		parse(&cl);

		// habrá que crear otro .c y .h para otras operaciones de asignacion de directorios

		freememory(&cl);

		if (strcmp(line, "EXIT") == 0) {
			exit(EXIT_SUCCESS);
		}

		/*if (feof(stdin)) {
		   // cerrar todo lo necesario y salir
		   freememory(&CommandLine, numwords);
		   exit(EXIT_SUCCESS);
		   } */
	}

	exit(EXIT_SUCCESS);
}
