#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include "parser.h"
 
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

// fijar los valores de cl->words en cl->commands
void
setcommands(CommandLine *cl){

	
	int i; 
	int posc;
	int possubc;

	int j; 
	int k;

	posc = 0;
	possubc = 0;
	for(i = 0; i < cl->numwords; i++){

		if(strcmp(cl->words[i], "|") != 0){

			// posición comando no ha superado al total de comandos
			if(posc < cl->numcommands){
				// posicion subcommandos no ha superado al total de la dim subcommando
				if(possubc < cl->numsubcommands[posc]){
					strcpy(cl->commands[posc][possubc], cl->words[i]);
					possubc++;
				}
			}
		}else{
			// hemos llegado a un pipe: 
			// 	- hay que aumentar posición del comando 
			// 	- hay que reiniciar la posición del subcomando
			posc++;
			possubc = 0;
		}
	}

	// traza 
	for(j = 0; j < cl->numcommands; j++){
		for(k = 0; k < cl->numsubcommands[j]; k++){

		fprintf(stderr,"soy el subcommando: %s\n", cl->commands[j][k]);		
		}
	}

}
	
void
handlepipes(CommandLine * cl){

	int i;
	int j;

	// inicializar los valores del char ***: FUNCIONAN
	cl->commands = (char ***)malloc(sizeof(char **) * (cl->numcommands));
	if (cl->commands == NULL) {
		perror("Error: dynamic memory cannot be allocated");
	}

	for(i = 0; i < cl->numcommands; i++){

		cl->commands[i] = (char **)malloc(sizeof(char *) * cl->numsubcommands[i]);

		if(cl->commands[i] == NULL){
			perror("Error: dynamic memory cannot be allocated");
		}
		for(j = 0; j < cl->numsubcommands[i]; j++){
			cl->commands[i][j] = (char *)malloc(sizeof(char) * MaxWord);
			if (cl->commands[i][j] == NULL) {
				perror("Error: dynamic memory cannot be allocated");
			}
		}

	}

	setcommands(cl);
}

// contar el número de comandos y sub commandos tiene cada comando
// almacenarlo de un array de ints para poder crear un char ***
void 
setnumcommands(CommandLine *cl){

	int i;
	int numcommands;
    int numsubcommands;

	cl->numsubcommands = (int *)malloc(sizeof(int) * (cl->numpipes+1));

	if(cl->numsubcommands == NULL){
		perror("Error: dynamic memory cannot be allocated");

	}

	numcommands = 0;
    numsubcommands = 0;

    for (i = 0; i < cl->numwords; i++) {
        	
		if (strcmp(cl->words[i], "|") == 0) {
			cl->numsubcommands[numcommands] = numsubcommands;
            numcommands++;
            numsubcommands = 0;
        } else {
            numsubcommands++;
        }
    }
	cl->numsubcommands[numcommands] = numsubcommands;
	cl->numcommands = numcommands + 1;

}
void 
casepipes(CommandLine * cl){

	
	int i;

	for (i = 0;  i < cl->numwords; i++){

		if (strcmp(cl->words[i], "|") == 0) {
			cl->numpipes++;
		}
	}

	if(cl->numpipes > 0){

		setnumcommands(cl);
		handlepipes(cl);
	}
}

void
parse(CommandLine * cl)
{
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

		//isenv(str)) 
		//(isequal(str)) 
}

void
freememory(CommandLine * cl)
{
	int i;
	//int j;

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

	if(cl->numpipes > 0){

		//libera cada subcomando
    	for (i = 0; i < cl->numcommands; i++) {

            for (int j = 0; j < cl->numsubcommands[i]; j++) {
                free(cl->commands[i][j]);
            }
			// libera comando
            free(cl->commands[i]);
    	}

    	// libera la estructura completa
    	free(cl->commands);
		free(cl->numsubcommands);
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
