#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parser.h"

// según se fijan cosas habrá que ir mirando los descriptores de
// fichero y demás 
// solo imprime el tipo de valor que es 
void
settype(CommandLine * cl, int val)
{

	// CommandLine DE MOMENTO NO SE USA 

	switch (val) {

	case PIPE:
		//cl->numpipes++;
		fprintf(stderr, "soy pipe\n");
		break;
	case STDINRED:
		fprintf(stderr, "soy redirección de entrada\n");
		break;
	case STDOUTRED:
		fprintf(stderr, "soy redirección de salida\n");
		break;

	case BACKGROUND:
		fprintf(stderr, "ejecuto en background\n");

		// eliminarlo de la string 
		// aumentar el valor del contador 
		break;

	case ENV:
		fprintf(stderr, "soy variable de entorno\n");
		break;

	case EQUAL:
		fprintf(stderr, "soy igual\n");
		break;

	case BUILTINCD:
		// se activará un booleano diciendo que la string es un builtnin
		fprintf(stderr, "soy cd\n");
		break;

	default:
		fprintf(stderr, "soy palabra\n");
		break;
	}
}


// Mira previamente si se encuentra al final de todo
int
isbg(char *str, int actualpos, int totalpos)
{

	//si es true return 1
	if (actualpos == (totalpos - 1)) {
		if (strcmp(str, "&") == 0) {
			return 1;
		}
	}

	return 0;
}

// comprueba que hay uno y sólo al principio de la palabra
int
isenv(char *str)
{

	int i;
	int len;

	len = strlen(str);

	// Si primer carácter no es '$' devuelve falso
	if (str[0] != '$') {
		return 0;
	}
	// Verifica si el carácter '$' aparece más de una vez
	for (i = 1; i < len; i++) {
		if (str[i] == '$') {
			return 0;
		}
	}

	// la cadena comienza con '$' y solo aparece una vez
	return 1;
}

// comprueba que hay uno y sólo en medio de la palabra
int
isequal(char *str)
{

	int i;
	int len;
	int times;

	len = strlen(str);

	// Si primer o último carácter es igual a '=' devuelve falso
	if ((str[0] == '=') || (str[len - 1] == '=')) {
		return 0;
	}

	times = 0;
	for (i = 1; i < len - 1; i++) {

		if (str[i] == '=') {
			times++;
		}

	}
	// comprueba que entre el segundo y 
	// penúltimo carácter haya solo un igual
	if (times == 1) {
		return 1;
	}
	return 0;
}

int
gettype(char *str, int actualpos, int totalpos)
{

	if (strcmp(str, "|") == 0) {
		return PIPE;
	}
	if (strcmp(str, "<") == 0) {
		return STDINRED;
	}
	if (strcmp(str, ">") == 0) {
		return STDOUTRED;
	}
	if (isbg(str, actualpos, totalpos)) {
		return BACKGROUND;
	}

	if (isenv(str)) {
		return ENV;
	}

	if (isequal(str)) {
		return EQUAL;
	}
	// ZONA DE BUILT-INS
	if (strcmp(str, "cd") == 0) {
		return BUILTINCD;
	}
	// más adelante incluir los opcionales

	return WORD;
}


// libera memoria y decrementa los valores
void 
elimstr(CommandLine *cl, int index) {
    
	int i;
	// indice fuera de lo buscado, no se puede eliminar
	if (index < 0 || index >= cl->numwords) {
        return;
    }

	// eliminar la memoria de la palabra 
    free(cl->words[index]);

	// desplazar las palabras a la izquierda
    for (i = index; i < cl->numwords - 1; i++) {
        cl->words[i] = cl->words[i + 1];
    }
    cl->numwords--;
}

// detecta si hay background, elimina dicha palabra de ese array 
void 
casebg(CommandLine * cl){

	// modificar esta función
	if (isbg(cl->words[cl->numwords-1], cl->numwords-1, cl->numwords)) {

		elimstr(cl,cl->numwords-1);
		cl->bg++;
		fprintf(stderr, "%d\n", cl->bg);

	}
}

void
parse(CommandLine * cl)
{

	int i;
	//int value;

	
	for (i = 0; i < cl->numwords; i++) {

		
		fprintf(stderr, "%s\n", cl->words[i]);
		
	}
	// 1º background (si ocurre eliminar la palabra y aumento el contador)
	casebg(cl);

	for (i = 0; i < cl->numwords; i++) {

		
		fprintf(stderr, "%s\n", cl->words[i]);
		
		//value = gettype(cl->words[i], i, cl->numwords);

		//settype(cl, value);
	}

	// 2º redirecciones 

		// 3º pipes y dividirlo en array de array de strings 
		// 4º el array de array de strings pueden ser: 
		// 	- variables de entorno 
		//  - sustituciones de variables de entorno 
		// 	- comandos
		// 		* ver si es built-in 
		// 		* fichero ejecutable en el dir trabajo
		// 		* fichero ejecutable que se encuentra en alguno de los directorios
		// 		  de la variable PATH

	//}
}

void
freememory(CommandLine * cl)
{

	int i;

	// liberamos el array de strings 
	for (i = 0; i < cl->numwords; i++) {
		//fprintf(stderr,"%s\n", CommandLine->words[i]);
		free(cl->words[i]);
	}
	free(cl->words);
}

void
tokenize(CommandLine *cl, char *line)
{

	int i;
	char *saveptr;
	char *token;

	cl->words =
	    (char **)malloc(sizeof(char *) * cl->numwords);
	if (cl->words == NULL) {
		perror("Error: dynamic memory cannot be allocated");
	}
	// inicializamos cada elemento del array
	for (i = 0; i < cl->numwords; i++) {
		cl->words[i] = (char *)malloc(sizeof(char) * MaxWord);
		if (cl->words[i] == NULL) {
			perror("Error: dynamic memory cannot be allocated");
		}

	}

	i = 0;
	token = strtok_r(line, " \t", &saveptr);
	// copiar el valor de token en el array dinámico
	strcpy(cl->words[i], token);

	i++;
	while (token != NULL && i < cl->numwords) {

		token = strtok_r(NULL, " \t", &saveptr);
		strcpy(cl->words[i], token);
		i++;
	}
}

int
getnumwords(char *line)
{

	int i;
	int numwords;
	int inword;

	i = 0;
	numwords = 0;
	// es falso
	inword = 0;
	while (line[i] != '\0') {

		if ((line[i] != ' ' && line[i] != '\t') && !inword) {
			numwords++;
			inword = 1;
		}

		if ((line[i] == ' ' || line[i] == '\t') && inword) {
			inword = 0;
		}

		i++;
	}
	return numwords;
}
