#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include "parser.h"

enum {
	ZeroArgs,
	MaxLine = 4*1024, // 4k  
};

void
usage(void)
{
	fprintf(stderr, "usage: ./shell\n");
	exit(EXIT_FAILURE);
}

// los valores de words
void
initcl(CommandLine *cl){

    cl->numwords = 0;
    cl->bg = 0;
    cl->stdired = 0;
    cl->stdored = 0;
	cl->numpipes = 0;
    cl->env = 0;
    cl->equal = 0;
	cl->status = 0;

}

int
main(int argc, char *argv[])
{

	char line[MaxLine];
	char *newline;
	int c;

	CommandLine cl;

	argc--;
	argv++;

	if (argc != ZeroArgs) {
		usage();
	}

	printf("Shell made by Julia López Augusto\n");
	printf("Write EXIT to leave\n");

	while (1) {

		initcl(&cl);

		if (fgets(line, MaxLine, stdin) == NULL) {
			break;
		}

		if (line[strlen(line) - 1] == '\n') {

			// Elimina el '\n' de la string
			newline = strrchr(line, '\n');

			if (newline != NULL) {
				// donde apunta newline poner un '\0'
				*newline = '\0';
			}

			cl.numwords = getnumwords(line);
			// tokeniza las palabras y las mete en un array de strings
			tokenize(&cl, line);
			// una vez tokenizado hay que distinguir cada caso
			parse(&cl);
			// paro de ejecutar porque en el parsing ha habido algun error
			// en concreto solo hay redirección y no fichero
			if(cl.status==PARSINGERROR){
				freememory(&cl);
				//fprintf(stderr,"red errr\n");
				continue;
			}

			fprintf(stderr,"sigo ejecutando\n");

			// habrá que crear otro .c y .h para otras operaciones de asignacion de directorios


			// trazas
			if(cl.status == INPUTRED){
				fprintf(stderr,"fichero de entrada: %s\n", cl.inred);

			}
			if(cl.status == OUTPUTRED){
				fprintf(stderr,"fichero de salida: %s\n", cl.outred);

			}
			if(cl.status == BOTHRED){
				fprintf(stderr,"fichero de entrada: %s\n", cl.inred);
				fprintf(stderr,"fichero de salida: %s\n", cl.outred);

			}

			freememory(&cl);
		
			if (strcmp(line, "EXIT") == 0) {
				exit(EXIT_SUCCESS);
			}

		}else {
			warnx("Exceeded path size");
			// elimina el contenido del buffer de entrada
            while ((c = getchar()) != '\n' && c != EOF);
			
		}
	}
	if(!feof(stdin)){
		errx(EXIT_FAILURE, "eof not reached\n");
	}
	if(ferror(stdin)){
		err(EXIT_FAILURE, "fgets got an error\n");
	}

	exit(EXIT_SUCCESS);
}
