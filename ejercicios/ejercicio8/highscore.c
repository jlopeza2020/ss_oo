#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

// quitar el usuario de la tabla es complejo 
// hcer un cleanup 
// el principal sale cuando acaben todos 
// no condicienes de carrera, los fifos funcionen correctamente 
// el cleanp ordenado sin condiciones de carrera y demás 

enum {
    MaxPlayers = 50,
    LineSz = 1024,
};

enum {
    Newplayer, 
    Delplayer,
    Highscore,
    Reset,
    Ncommnads,
}; 

char  *commands[Ncommnads] = {"newplayer", "delplayer", "highscore", "reset"};

struct Command{
    char *command;
    char *arg; 
};
typedef struct Command Command;


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
checkcmd(char *cmd){

    int i;

    for(i = 0; i < Ncommnads; i++){

        if(strcmp(cmd, commands[i]) == 0){
            return i;
        }
    }
    return -1;

}

// preguntar si isalnum se puede usar 
// ASCII:
// A = 65, Z = 90
// a = 97, z = 122
// 0 = 48, 9 = 57

int 
isletter(char letter){
    return ((letter >= 'A' && letter <= 'Z') || (letter >= 'a' && letter <= 'z'));
}
int 
isnumber(char letter){
    return (letter >= '0' && letter <= '9');
}
int 
isname(char *word) {

    while (*word) {

        if(!isletter(*word) && !isnumber(*word)){
            return 0;
        }
        word++;
    }
    // si es un carácter alfanumérico
    return 1; 
}

int 
checkarg(char *arg, int value){

    int valuecmd;
    
    valuecmd = value;
    
    switch (value) {
	// obligatoriamente recibe un nombre con numeros y letras
    case Newplayer:
        if(strcmp(arg, "nocommand") == 0 || !isname(arg)){
            valuecmd = -1;
        }
		break;
    // obligatoriamente recibe un nombre con numeros y letras
	case Delplayer:
        if(strcmp(arg, "nocommand") == 0 || !isname(arg)){
            valuecmd = -1;
        }
		break;
    // no recibe ningún argumento más
    case Highscore:
        if(strcmp(arg, "nocommand") != 0){
            valuecmd = -1;
        }
		break;
    // opcionalmente recibe un nombre con numeros y letras
	case Reset:

        if(strcmp(arg, "nocommand") != 0 && !isname(arg)){
            valuecmd = -1;
        }
        break;

	default:
        valuecmd = -1;
	}

    return valuecmd;
}

int
getkindcommand(Command *cl, char *line){

    char *saveptr;
	char *token;
    //char *aux;

    int valueword;
    int valuecmd;

    cl->command = (char *)malloc(sizeof(char) * LineSz);
    cl->arg = (char *)malloc(sizeof(char) * LineSz);

    if (cl->command == NULL || cl->arg == NULL) {
		errx(EXIT_FAILURE,"Error: dynamic memory cannot be allocated");
	}

    if(getnumwords(line) > 2){
        return -1;
    }


    token = strtok_r(line, " \t" , &saveptr);
    if(token == NULL){
        //free(aux);
        free(cl->command);
        free(cl->arg);
        return -1;
    }
    // solo nos quedamos con la primera palabra de la tokenización
    strcpy(cl->command, token);

    // solo nos quedamos con el primero de la tokenización
    valueword = checkcmd(cl->command);

    token = strtok_r(NULL, " \t" , &saveptr);

    if(token == NULL){
        strcpy(cl->arg, "nocommand");
    }else{
        strcpy(cl->arg, token);
    }
    // tengo que comprobar el segundo argumento 
    valuecmd = checkarg(cl->arg, valueword);    

    //free(aux);

    return valuecmd;
}

void 
treatkind(char *name, int kind){

    
    switch (kind) {
    case Newplayer:
        //addplayer(name);
        fprintf(stderr, "soy newplayer: %s \n", name);
		break;
	case Delplayer:
        //deleteplayer(name);
        fprintf(stderr, "soy delplayer: %s\n", name);
		break;
    case Highscore:
        //highscore();
        fprintf(stderr, "soy highscore: %s\n", name);
		break;
	case Reset:
        //reset(name);
        fprintf(stderr, "soy reset: %s \n", name);
        break;

	default:
	}

}


int 
main(int argc, char *argv[]){

    char line[LineSz];
    int c;
    int kind;
    Command cl;

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

        // Comprobar que el input se trate de algún 
        // comando definido y siga su forma correcta
        kind = getkindcommand(&cl,line);
        // si el comando es incorrecto
        if(kind < 0){
            fprintf(stderr, "Incorrect command\n");
            free(cl.command);
            free(cl.arg);
            continue;
        }

        treatkind(cl.arg,kind);

        //fprintf(stderr, "soy el comando num: %d\n", kind);
        // si el valor  es alguno definiddo hacer las operacioens que corresponda
        // hacer una lista enlazada 
        free(cl.command);
        free(cl.arg);
    }

    if(!feof(stdin)){
        errx(EXIT_FAILURE, "eof not reached\n");
    }
    if(ferror(stdin)){
        errx(EXIT_FAILURE, "read error");
    }

    exit(EXIT_SUCCESS);
}