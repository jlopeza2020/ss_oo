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

int
main(int argc, char *argv[])
{

	char line[MaxLine];

	//int numwords;
	CheckInput checkinput;

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
		// elimina el salto de línea al final
		line[strcspn(line, "\n")] = 0;  // pone el último byte \0 es como strchr 

		checkinput.numwords = getnumwords(line);
		tokenize(&checkinput, line);
		// una vez tokenizado hay que distinguir cada caso
		parse(&checkinput);

		freememory(&checkinput);

		if (strcmp(line, "EXIT") == 0) {
			exit(EXIT_SUCCESS);
		}

		/*if (feof(stdin)) {
		   // cerrar todo lo necesario y salir
		   freememory(&checkinput, numwords);
		   exit(EXIT_SUCCESS);
		   } */
	}

	exit(EXIT_SUCCESS);
}
