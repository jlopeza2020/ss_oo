#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include "common.h"
#include "parser.h"
#include "executor.h"

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
	cl->numcommands = 0;
    cl->bg = 0;
    cl->stdired = 0;
    cl->stdored = 0;
	cl->numpipes = 0;
	cl->status = 0;
	cl->statusred = 0;
}

int
main(int argc, char *argv[])
{

	char line[MaxLine];
	char *newline;
	int c;
	int i;
	int j,k;

	// así está bien
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

		if (line[strlen(line) - 1] != '\n'){
            fprintf(stderr, "Exceeded path size\n");
            // Limpia el buffer de entrada
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

		// Elimina el '\n' de la string
		newline = strrchr(line, '\n');

		if (newline != NULL) {
			// donde apunta newline poner un '\0'
			*newline = '\0';
		}

		if (strcmp(line, "EXIT") == 0) {
			exit(EXIT_SUCCESS);
		}

		cl.numwords = getnumwords(line);

		if (cl.numwords > MaxWords){
			fprintf(stderr,"Line too long\n");
			continue;
		}
		if(cl.numwords == 0){
			continue;
		}

		// tokeniza las palabras y las mete en un array de strings
		tokenize(&cl, line);
			
		// una vez tokenizado hay que distinguir cada caso
		parse(&cl);

		// paro de ejecutar porque en el parsing ha habido algun error
		if(cl.status==PARSINGERROR){
			freememory(&cl);
			fprintf(stderr,"Syntax error near unexpected token\n");
			// paso a la siguiente ejecución
			continue;
		}

		// trazas
		for(i = 0; i < cl.numwords; i++){
			fprintf(stderr, "Palabra: %s\n", cl.words[i]);
		}

		if(cl.numpipes > 0){
			for(j = 0; j < cl.numcommands; j++){
				for(k = 0; k < cl.numsubcommands[j]; k++){

					fprintf(stderr,"soy el subcommando: %s\n", cl.commands[j][k]);		
				}
			}

		}
		if(cl.bg){
			fprintf(stderr, "Hay bg\n");

		}

		// trazas
		if(cl.statusred == INPUTRED){
			fprintf(stderr,"fichero de entrada: %s\n", cl.inred);
		}
		if(cl.statusred == OUTPUTRED){
			fprintf(stderr,"fichero de salida: %s\n", cl.outred);
		}
		if(cl.statusred == BOTHRED){
			fprintf(stderr,"fichero de entrada: %s\n", cl.inred);
			fprintf(stderr,"fichero de salida: %s\n", cl.outred);
		}

		// A partir de aquí todo está parseado y decidido para lo que queremos ejecutar
		// escribirlo en executor.c 
		findcommands(&cl);

		if(cl.status==FINDERROR){
			freememory(&cl);
			fprintf(stderr,"Command not found\n");
			// paso a la siguiente ejecución
			continue;
		}

		//executecommands(&cl);

		freememory(&cl);
	}
	if(!feof(stdin)){
		errx(EXIT_FAILURE, "eof not reached\n");
	}
	if(ferror(stdin)){
		err(EXIT_FAILURE, "fgets got an error\n");
	}

	exit(EXIT_SUCCESS);
}
