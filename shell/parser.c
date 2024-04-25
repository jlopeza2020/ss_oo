#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "parser.h"

/*
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
*/
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
		fprintf(stderr,"Missed redirection file\n");

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

	//int j; 
	//int k;

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
	/*for(j = 0; j < cl->numcommands; j++){
		for(k = 0; k < cl->numsubcommands[j]; k++){

		fprintf(stderr,"soy el subcommando: %s\n", cl->commands[j][k]);		
		}
	}*/

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
	//caseenv 
	casebg(cl);
	casered(cl);
	casepipes(cl);
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
		// libero el array de ints
		free(cl->numsubcommands);
	}
}

void 
handlespecialchars(CommandLine *cl, char *word, int *pos) {
    
	/*char *saveptr;
    char *token;
	
	token = strtok_r(word, "|", &saveptr);
	    
    while (token != NULL && *pos < cl->numwords) {
        strcpy(cl->words[*pos], token);
        *pos = *pos + 1;

        // Add "|" to separate tokens if not the last one
        if (*pos < cl->numwords) {
            strcpy(cl->words[*pos], "|");
            *pos = *pos + 1;
        }
        
        token = strtok_r(NULL, "|", &saveptr);
    }*/

	//char word[] = "hola|a<o";
    //char tokens[100][100]; // Array para almacenar los tokens
    //int token_count = 0;
    int word_index = 0;
    int token_index = 0;

	fprintf(stderr, "pos: %d\n", *pos);
    // Recorre la palabra caracter por caracter
    while (word[word_index] != '\0') {
        // Si el caracter actual es '|', termina el token actual y avanza al siguiente
        if (word[word_index] == '|') {
            cl->words[*pos][token_index] = '\0'; // Agrega un terminador de cadena al final del token
            *pos = *pos+1; // Incrementa el contador de tokens
            token_index = 0; // Reinicia el índice del token para el siguiente token
            // Agrega el delimitador '|' como un token adicional
            //*pos = *pos +1;
			strcpy(cl->words[*pos], "|");
			*pos = *pos +1;
        } else {
            // Si no es un delimitador, agrega el caracter al token actual
            cl->words[*pos][token_index++] = word[word_index];
        }
        // Avanza al siguiente caracter de la palabra
        word_index++;
    }
    // Termina el último token
    cl->words[*pos][token_index] = '\0';
    *pos = *pos +1; // Incrementa el contador de tokens para el último token
}

/*void 
handlespecialchars(CommandLine *cl, char *word, int *pos){


	char *saveptr;
	char *token;
	
	token = strtok_r(word, "|" , &saveptr);
	fprintf(stderr, "token: %s\n", token);
	fprintf(stderr, "saveptr: %s\n", saveptr);
	strcpy(cl->words[*pos], token);

	while (token != NULL && *pos < cl->numwords) {

		*pos = *pos+1;
		strcpy(cl->words[*pos], "|");

		token = strtok_r(NULL, "|", &saveptr);
		strcpy(cl->words[*pos], token);
		*pos = *pos+1;
		fprintf(stderr, "token2: %s\n", token);
		fprintf(stderr, "saveptr2: %s\n", saveptr);
	}


	long long init;
	long long end;
	int i;
	// hay dentro de la palabra un |

	init = 0;
	end = 0;

	while (word[end] != '\0') {

		fprintf(stderr, " %c\n", word[end]);

		if(word[end] == '|'){

			for(i=init; i < end; i++){
				//strcat(dest, origen) siempre pone el '/0'
				//strcat(cl->words[*pos], word[i]);
				cl->words[*pos][i] = word[i];
				fprintf(stderr, "%c\n", cl->words[*pos][i] );

			}
			cl->words[*pos][end] = '\0';
			init = end;

			*pos = *pos + 1;
			strcpy(cl->words[*pos], "|");
			*pos = *pos + 1;

		}
		end++;
	}*/
	/*token = strtok_r(word, "|", &saveptr);
	if(saveptr !=  NULL){
		fprintf(stderr, "hay un | en %s\n", token);
	}*/

	// CAMBIAR LA FORMA DE PLANTEARLO
//}
// tokeniza y almacena en un array de strings todos los elementos
void
tokenize(CommandLine *cl, char *line)
{
	int i;
	char *saveptr;
	char *token;
	char *aux;

	aux = (char *)malloc(sizeof(char) * MaxWord);
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
	// cambiar el tokenizado !!!!
	// tener en cuenta los del | > y < si están pegadas
	// mirar dentro de los tokens y comprobar si no es NULL en valos buscado
	token = strtok_r(line, " \t" , &saveptr);
	strcpy(aux, token);

	// copiar el valor de token en el array words
	handlespecialchars(cl, aux, &i);

	//strcpy(cl->words[i], token);
	//i++;
	/*while (token != NULL && i < cl->numwords) {

		token = strtok_r(NULL, " \t", &saveptr);
		strcpy(cl->words[i], token);
		i++;
	}*/
	free(aux);
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
	// indica si nos encontramos dentro de una palabra
	inword = 0;
	while (line[i] != '\0') {

		// si el carácter actual no es ninguno  de los que buscamos
		// y no estamos dentro de una palabra, aumentamos el número de palabra
		// y decimios que nos encontramos dentro de una palabra
		if (line[i] != ' ' && line[i] != '\t' &&  line[i] != '|' && line[i] != '<' && line[i] != '>') {

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
		
		// si a lo largo de la linea hay algún caracter que buscamos:
		// inicializamos una palabra nueva 
		if(line[i] == '|' || line[i] == '<' || line[i] == '>'){
			numwords++;
			inword = 0;
		}

		i++;
	}
	return numwords;
}
