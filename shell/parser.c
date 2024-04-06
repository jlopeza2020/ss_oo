#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>

#include "parser.h"
 
/*void
settype(CommandLine * cl, int val)
{

	// CommandLine DE MOMENTO NO SE USA 

	switch (val) {

	case PIPE:
		//cl->numpipes++;
		fprintf(stderr, "soy pipe\n");
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
}*/

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

// lo uso como trazas
/*int
gettype(char *str, int actualpos, int totalpos)
{

	if (strcmp(str, "|") == 0) {
		return PIPE;
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
}*/

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

// detecta si hay background y si ocurre: 
// elimina dicha palabra de ese array 
void 
casebg(CommandLine * cl){

	if (isbg(cl)) {
		elimstr(cl,cl->numwords-1);
		cl->bg++;
	}
}

int
isbg(CommandLine *cl)
{
	if (strcmp(cl->words[cl->numwords-1], "&") == 0) {
		return 1;
	}
	return 0;
}

int 
isstr(char *word) {
    // Verifica si el argumento es un puntero nulo
    if (word == NULL) {
        return 0; 
    }

    if (strlen(word) > 0 && word[0] != '\0') {
        return 1; // Es un string
    } else {
        return 0;
    }
}

int
isred(CommandLine *cl, char *typered)
{
	if(cl->numwords >= 2){

		if (strcmp(cl->words[cl->numwords-2], typered) == 0) {
			if(isstr(cl->words[cl->numwords-1])){
				return 1; // hay red y es una str
			}
		}
	}

	// caso en el que solo se encuentre redirección al final
	if (strcmp(cl->words[cl->numwords-1], typered) == 0) {
		warnx("Missed redirection file\n");
		cl->status = PARSINGERROR;
	}

	return 0;
}

void
handlered(CommandLine *cl, char *file, int value, int status){

	// almacenar la string en algún lado
	strcpy(file,cl->words[cl->numwords-1]);

	// elimina la string
	elimstr(cl,cl->numwords-1);
	// elimina <
	elimstr(cl,cl->numwords-1);
	value++;
	cl->status += status; 
}


void 
casered(CommandLine *cl){

	// Pueden aparecer en cualquier orden
	for (int i = 0; i < cl->numwords; i++) {
		if (isred(cl, "<")){
			
			cl->inred = (char *)malloc(sizeof(char) * MaxWord);
			if (cl->inred == NULL) {
				perror("Error: dynamic memory cannot be allocated");
			}
			handlered(cl, cl->inred, cl->stdired++, INPUTRED);
		}

		if(isred(cl, ">")){

			// almacenar la string en algún lado
			cl->outred = (char *)malloc(sizeof(char) * MaxWord);
			if (cl->outred == NULL) {
				perror("Error: dynamic memory cannot be allocated");
			}
			handlered(cl, cl->outred, cl->stdored++, OUTPUTRED);

		}

		if(cl->status == PARSINGERROR){
			break;
		}
	}
}

void 
casepipes(CommandLine * cl){

	int i;

	for (i = 0;  i < cl->numwords; i++){

		if (strcmp(cl->words[i], "|") == 0) {

			cl->numpipes++;
		}

	}

	// handlepipes
	// crear un array char *** con numpipes + 1 posiciones
	// llenarlo con cada parte

}

void
parse(CommandLine * cl)
{
	int i;

	// uso de traza
	for (i = 0; i < cl->numwords; i++) {

		fprintf(stderr, "%s", cl->words[i]);
	}
	// 1º background (si ocurre eliminar la palabra y aumento el contador)
	casebg(cl);

	// 2º redirecciones 
	casered(cl);

	// 3º pipes y dividirlo en array de array de strings 

	casepipes(cl);

		// 4º el array de array de strings pueden ser: 
		// 	- variables de entorno 
		//  - sustituciones de variables de entorno 
		// 	- comandos
		// 		* ver si es built-in 
		// 		* fichero ejecutable en el dir trabajo
		// 		* fichero ejecutable que se encuentra en alguno de los directorios
		// 		  de la variable PATH
	
	// uso de traza
	for (i = 0; i < cl->numwords; i++) {
		fprintf(stderr, "%s\n", cl->words[i]);
	}
}

void
freememory(CommandLine * cl)
{
	int i;

	// liberamos el array de strings 
	for (i = 0; i < cl->numwords; i++) {
		free(cl->words[i]);
	}
	free(cl->words);

	// solo se ha creado el de entrada 
	if(cl->status == INPUTRED){
		free(cl->inred);
	}

	// solo se ha credo el de salida
	if(cl->status == OUTPUTRED){
		free(cl->outred);
	}
	// se han creado los dos
	if (cl->status == BOTHRED){
		free(cl->inred);
		free(cl->outred);
	}
}

// tokeniza y almacena en un array de strings todos los elementos
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
	// copiar el valor de token en el array words
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
	// inword se inicializa como falso
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
