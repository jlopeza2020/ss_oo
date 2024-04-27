#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>


enum {
    LineSz = 512,
};

enum {
    Newplayer, 
    Delplayer,
    Highscore,
    Reset,
    Ncommnads,
}; 

char  *commands[Ncommnads] = {"newplayer", "delplayer", "highscore", "reset"};

/*struct CheckInput{
    int command[Ncommnads];
    long long size; 
};
typedef struct CheckInput CheckInput;*/


void
usage(void)
{
	fprintf(stderr, "usage: ./highscore\n");
	exit(EXIT_FAILURE);
}


int
getnumwords(char *line)
{
	int i;
	int numwords;
	int inword;

	i = 0;
	numwords = 0;
	// inword se inicializa como falso e
	// indica si nos encontramos dentro de una palabra
	inword = 0;
	while (line[i] != '\0') {

		// si el carácter actual no es ninguno de los que buscamos
		// y no estamos dentro de una palabra, aumentamos el número de palabra
		// y decidimios que nos encontramos dentro de una palabra
		if (line[i] != ' ' && line[i] != '\t' ) {

			if(!inword){
				numwords++;
				inword = 1;
			}

		}
		// si el caracter es espacio o tabulador y si ha estado 
		// dentro de una palabra, ponemos que acabamos de salir de ella  
		if (line[i] == ' ' || line[i] == '\t') {
			if(inword){
				inword = 0;

			}
		}
		
		i++;
	}
	return numwords;
}


int
checkword(char *cmd){

    int i;

    for(i = 0; i < Ncommnads; i++){

        if(strcmp(cmd, commands[i]) == 0){
            return i;
        }
    }
    return -1;

}


// preguntar si isalnum se puede usar 
int 
isname(const char *palabra) {
    while (*palabra) {
        if (!((*palabra >= 'A' && *palabra <= 'Z') || (*palabra >= 'a' && *palabra <= 'z') || (*palabra >= '0' && *palabra <= '9'))) {
            return 0; // No es alfanumérico
        }
        palabra++;
    }
    return 1; // Es alfanumérico
}

int 
checkcmd(char *arg, int value){

    int valuecmd;
    
    valuecmd = value;
    
    switch (value) {

	// obligatoriamente recibe un nombre con numeros y letras
    case Newplayer:
        if(!isname(arg)){
            valuecmd = -1;
        }
		break;

    // obligatoriamente recibe un nombre con numeros y letras
	case Delplayer:
        if(!isname(arg)){
            valuecmd = -1;
        }
		break;
        
    // no recibe ningún argumento más
    case Highscore:
        if(strcmp(arg, "nocommand") == 0){
            valuecmd = value;
        }else{
            valuecmd = -1;

        }
		break;

    // opcionalmente recibe un nombre con numeros y letras
	case Reset:

        if(strcmp(arg, "nocommand") == 0){
            valuecmd = value;
        }else{
            if(!isname(arg)){
                valuecmd = -1;
            }
        }
        break;

	default:
        valuecmd = -1;

	}

    return valuecmd;
}
int
getkindcommand(char *line){

    char *saveptr;
	char *token;
    char *aux;

    int valueword;
    int valuecmd;


    if(getnumwords(line) > 2){
        return -1;
    }
    
    aux = (char *)malloc(sizeof(char) * LineSz);

    if (aux == NULL) {
		errx(EXIT_FAILURE,"Error: dynamic memory cannot be allocated");
	}

    token = strtok_r(line, " \t" , &saveptr);
    if(token == NULL){
        free(aux);
        return -1;
    }
    
    // solo nos quedamos con la primera palabra de la tokenización
    strcpy(aux, token);

    // solo nos quedamos con el primero de la tokenización
    //printf("%s\n", aux);
    valueword = checkword(aux);

    token = strtok_r(NULL, " \t" , &saveptr);
    if(token == NULL){
        strcpy(aux, "nocommand");

    }else{
        strcpy(aux, token);
    }
    // tengo que comprobar el segundo argumento 
    valuecmd = checkcmd(aux, valueword);    

    free(aux);


    return valuecmd;
}

int 
main(int argc, char *argv[]){

    char line[LineSz];
    int c;
    int kind;

    argc--;
    argv++;

    if(argc != 0){
        usage();
    }

    while (fgets(line, LineSz, stdin) != NULL){

        // La línea excede el tamaño máximo
        if (line[strlen(line) - 1] != '\n'){
            fprintf(stderr, "Exceeded path size\n");
            // Limpia el buffer de entrada
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        line[strlen(line) -1] = '\0';

        // Comprobar que el input se trate de algún comando definido
        kind = getkindcommand(line);
        // si el comando es incorrecto
        if(kind < 0){

            fprintf(stderr, "Incorrect command\n");
            //while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        fprintf(stderr, "soy el comando num: %d\n", kind);
        // si el valor  es alguno definiddo hacer las operacioens que corresponda

    }

    if(!feof(stdin)){
        errx(EXIT_FAILURE, "eof not reached\n");
    }
    if(ferror(stdin)){
        errx(EXIT_FAILURE, "read error");
    }

    exit(EXIT_SUCCESS);
}