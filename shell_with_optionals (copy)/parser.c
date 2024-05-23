#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <glob.h>
#include "common.h"
#include "parser.h"

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

void
setequal(char *str)
{

	char *token;
	char *saveptr;
	char name[MaxWord];
	char value[MaxWord];

	token = strtok_r(str, "=", &saveptr);
	strcpy(name, token);
	strcpy(value, saveptr);

	// el valor de overwrite es 1 porque sino, no 
	// se va a sobreescribir el valor si se modifica
	if (setenv(name, value, 1) != 0) {
		fprintf(stderr, "Env var %s could not be set\n", name);
	}
}

// si hay pipes, se tomará los iguales como palabras
// si en la misma palabra hay más de un igual también se tratará como palabras
void
caseequal(CommandLine *cl)
{

	long long i;

	if (cl->numpipes == 0 && cl->status != PARSINGERROR) {

		for (i = 0; i < cl->numwords; i++) {

			if (isequal(cl->words[i])) {
				// hacer la sustitución correspondiente
				setequal(cl->words[i]);
				elimstr(cl, i);
				// al moverse todo una posición a la izquierda, 
				// es necesario decrementar dicho valor
				i--;
			}
		}
	}
}

// Comprueba que la palabra contiene un $ y si es $ solo, 
// no se trata como una variable de entorno
int
isenv(char *str)
{

	int i;
	int len;

	len = strlen(str);

	// si el primer caracter es '$' y no hay nada más
	if (str[0] == '$' && len == 1) {
		return 0;
	}
	// Verifica si el carácter '$' aparece a lo largo de la palabra
	for (i = 0; i < len; i++) {
		if (str[i] == '$') {
			return 1;
		}
	}

	// la cadena comienza con '$' y solo aparece una vez
	return 0;
}

void
elimfirstchar(char *word)
{
	int i;
	int len;

	len = strlen(word);

	// Mover cada carácter un índice a la izquierda
	for (i = 0; i < len; i++) {
		word[i] = word[i + 1];
	}
}

void
setenvvar(CommandLine *cl, char *str)
{

	char *value;

	value = getenv(str);
	if (value != NULL) {
		strcpy(str, value);
	} else {
		fprintf(stderr, "error: var %s does not exist\n", str);
		cl->status = PARSINGERROR;
	}

}

// sustitución de $ 
// Solo funciona para $cualquiercosa (menos |, >,<,&)
// Si aparece $ a solas se trata como una palabra normal.
// En el resto de casos funciona perfectamente
void
caseenv(CommandLine *cl)
{
	long long i;
	char *aux;
	char *original;
	char *pos;
	char tmpenvvar[MaxWord];
	char *firstenvironchar;

	for (i = 0; i < cl->numwords; i++) {

		if (isenv(cl->words[i])) {
			aux = (char *)malloc(sizeof(char) * MaxWord);
			if (aux == NULL) {
				err(EXIT_FAILURE,
				    "Error: dynamic memory cannot be allocated");
			}
			// inicializa aux con '/0' para que no de errores de memoria sin inicializar
			aux[0] = '\0';

			// Apunta al principio de la palabra
			original = cl->words[i];
			// apunta al final de la palabra
			pos = strchr(original, '$');

			while (pos != NULL) {
				// se va copiando lo que no tiene asignado variable de entorno
				strncat(aux, original, pos - original);

				// se aumenta el valor de posición para que no apunte a '$'
				pos++;
				// se deja en el primer caracter de la variable de entorno
				firstenvironchar = pos;
				// se va aumentando el puntero hasta que llegue a final
				// de linea o a otra variable de entorno
				while (*pos != '\0' && *pos != '$') {
					pos++;
				}
				// en tmpenvvar almacenamos temporalmente la variable de entorno
				strncpy(tmpenvvar, firstenvironchar,
					pos - firstenvironchar);
				tmpenvvar[pos - firstenvironchar] = '\0';

				// copiamos a tmpenvvar el valor de la variable de entorno
				setenvvar(cl, tmpenvvar);
				// concatenamos a aux, el valor de la variable de entorno
				strcat(aux, tmpenvvar);

				// Para seguir buscando más, actualizamos la posición de original a donde apunta pos
				original = pos;

				// Se encuentra el siguiente string 
				pos = strchr(original, '$');
			}

			// Copia cualquier cosa que falte del último '$'
			strcat(aux, original);
			// Copiamos la string final a la palabra actual
			strcpy(cl->words[i], aux);
			free(aux);
		}
	}
}

// libera memoria y decrementa los valores
void
elimstr(CommandLine *cl, long long index)
{

	long long i;

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
casebg(CommandLine *cl)
{

	if (isbg(cl)) {
		elimstr(cl, cl->numwords - 1);
		cl->bg++;
	}
}

int
isbg(CommandLine *cl)
{
	if (strcmp(cl->words[cl->numwords - 1], "&") == 0) {
		return 1;
	}
	return 0;
}

int
isambiguoischar(char letter)
{
	return (letter == '<' || letter == '>' || letter == '|');
}

int
isname(char *word)
{

	while (*word) {

		if (isambiguoischar(*word)) {
			return 0;
		}
		word++;
	}
	// si es un carácter que buscamos
	return 1;
}

int
isstr(char *word)
{
	// Verifica si el argumento es un puntero nulo
	if (word == NULL) {
		return 0;
	}

	if (strlen(word) > 0 && isname(word)) {
		// Es un string
		return 1;
	} else {
		return 0;
	}
}

int
isred(CommandLine *cl, char *typered)
{
	if (cl->numwords >= 2) {

		if (strcmp(cl->words[cl->numwords - 2], typered) == 0) {
			if (isstr(cl->words[cl->numwords - 1])) {
				return 1;	// hay red y es una str
			}
		}
	}
	// caso en el que solo se encuentre redirección al final:
	// falta el fichero al final
	if (strcmp(cl->words[cl->numwords - 1], typered) == 0) {
		fprintf(stderr, "Missed redirection file\n");
		cl->status = PARSINGERROR;
	}

	return 0;
}

void
setred2struct(CommandLine *cl, char *file, int value, int status)
{

	// almacenar la string en algún lado
	strcpy(file, cl->words[cl->numwords - 1]);

	// elimina la string
	elimstr(cl, cl->numwords - 1);
	// elimina <
	elimstr(cl, cl->numwords - 1);
	value++;
	cl->statusred += status;
}

// si recibo una linea estilo: echo hola > eje > eje2
// tomará como descriptor de fichero eje2  y '>', 'eje' se 
// tomarán como palabras
void
casered(CommandLine *cl)
{

	long long i;

	// Pueden aparecer en cualquier orden de '<' o '>' y viceversa
	for (i = 0; i < cl->numwords; i++) {
		if (isred(cl, "<") && cl->inrednum == 0) {

			cl->inred = (char *)malloc(sizeof(char) * MaxWord);
			if (cl->inred == NULL) {
				err(EXIT_FAILURE,
				    "Error: dynamic memory cannot be allocated");
			}
			setred2struct(cl, cl->inred, cl->inrednum++, INPUTRED);
		}

		if (isred(cl, ">") && cl->outrednum == 0) {

			// almacenar la string en algún lado
			cl->outred = (char *)malloc(sizeof(char) * MaxWord);
			if (cl->outred == NULL) {
				err(EXIT_FAILURE,
				    "Error: dynamic memory cannot be allocated");
			}
			setred2struct(cl, cl->outred, cl->outrednum++,
				      OUTPUTRED);
		}

		if (cl->status == PARSINGERROR) {
			break;
		}
	}
}

// fijar los valores de cl->words en cl->commands si hay pipes entre medias
void
setcommands(CommandLine *cl)
{

	long long i;
	long long posc;
	long long possubc;

	posc = 0;
	possubc = 0;
	for (i = 0; i < cl->numwords; i++) {

		if (strcmp(cl->words[i], "|") != 0) {

			// posición comando no ha superado al total de comandos
			if (posc < cl->numcommands) {
				// posicion subcommandos no ha superado al total de la dim subcommando
				if (possubc < cl->numsubcommands[posc]) {
					strcpy(cl->commands[posc][possubc],
					       cl->words[i]);
					possubc++;
				}
			}
		} else {
			// hemos llegado a un pipe: 
			//      - hay que aumentar posición del comando 
			//      - hay que reiniciar la posición del subcomando
			posc++;
			possubc = 0;
		}
	}
}

void
handlepipes(CommandLine *cl)
{

	long long i;
	long long j;

	// inicializa los valores del char ***
	cl->commands = (char ***)malloc(sizeof(char **) * (cl->numcommands));
	if (cl->commands == NULL) {
		err(EXIT_FAILURE, "Error: dynamic memory cannot be allocated");
	}

	for (i = 0; i < cl->numcommands; i++) {

		cl->commands[i] =
		    (char **)malloc(sizeof(char *) * cl->numsubcommands[i]);

		if (cl->commands[i] == NULL) {
			err(EXIT_FAILURE,
			    "Error: dynamic memory cannot be allocated");
		}
		for (j = 0; j < cl->numsubcommands[i]; j++) {
			cl->commands[i][j] =
			    (char *)malloc(sizeof(char) * MaxWord);
			if (cl->commands[i][j] == NULL) {
				err(EXIT_FAILURE,
				    "Error: dynamic memory cannot be allocated");
			}
		}

	}

	setcommands(cl);
}

// contar el número de comandos y subcommandos tiene cada comando
// almacenarlo de un array de ints para poder crear un char ***
void
setnumcommands(CommandLine *cl)
{

	long long i;
	long long numcommands;
	long long numsubcommands;

	cl->numsubcommands =
	    (long long *)malloc(sizeof(long long) * (cl->numpipes + 1));

	if (cl->numsubcommands == NULL) {
		err(EXIT_FAILURE, "Error: dynamic memory cannot be allocated");

	}

	numcommands = 0;
	numsubcommands = 0;

	for (i = 0; i < cl->numwords; i++) {

		if (strcmp(cl->words[i], "|") == 0) {
			cl->numsubcommands[numcommands] = numsubcommands;
			numcommands++;
			// se reinicia el número de subcomandos 
			// porque hemos llegado a un pipe
			numsubcommands = 0;
		} else {
			numsubcommands++;
		}
	}
	cl->numsubcommands[numcommands] = numsubcommands;
	cl->numcommands = numcommands + 1;

}

// comprueba que no haya pipes seguidos o puestos al principio
// o al final de la línea
void
checkpipessyntax(CommandLine *cl)
{

	long long i;

	if (strcmp(cl->words[0], "|") == 0
	    || strcmp(cl->words[cl->numwords - 1], "|") == 0) {
		cl->status = PARSINGERROR;
	}
	// posición 0 a la penúltima
	for (i = 0; i < cl->numwords - 1; i++) {
		if (strcmp(cl->words[i], "|") == 0
		    && strcmp(cl->words[i + 1], "|") == 0) {
			cl->status = PARSINGERROR;
		}
	}
}

void
casepipes(CommandLine *cl)
{

	long long i;

	checkpipessyntax(cl);

	if (cl->status != PARSINGERROR) {

		// cuenta el número de pipes
		for (i = 0; i < cl->numwords; i++) {

			if (strcmp(cl->words[i], "|") == 0) {
				cl->numpipes++;
			}
		}

		if (cl->numpipes > 0) {

			setnumcommands(cl);
			handlepipes(cl);
		}
	}
}

// OPCIONAL 1
void
casehere(CommandLine *cl)
{

	if (cl->bg == 0 && cl->inrednum == 0 && cl->outrednum == 0) {

		if (strcmp(cl->words[cl->numwords - 1], "HERE{") == 0) {
			// hay un here document
			cl->heredoc++;
			elimstr(cl, cl->numwords - 1);
		}
	}
}

// OPCIONAL 3
void
setglobbing(CommandLine *cl, char *token, long long *pos)
{
	glob_t glob_result;
	size_t i;
	long long newsize;

	// glob ha tenido éxito
	if (glob(token, GLOB_BRACE | GLOB_TILDE, NULL, &glob_result) == 0) {
		// se quita uno por lo que ocupa el globbing 
		newsize = cl->numwords + glob_result.gl_pathc - 1;

		cl->words =
		    (char **)realloc(cl->words, newsize * sizeof(char *));
		if (cl->words == NULL) {
			err(EXIT_FAILURE, "Error: realloc failed\n");
		}
		// se libera la posición que contiene los carcteres de globbing
		free(cl->words[*pos]);
		// Desplazar elementos para hacer espacio para el nuevo contenido
		for (i = cl->numwords - 1; i >= *pos + 1; i--) {
			cl->words[i + glob_result.gl_pathc - 1] = cl->words[i];
		}
		// Insertar el nuevo contenido en el array
		for (i = 0; i < glob_result.gl_pathc; i++) {
			cl->words[*pos + i] = strdup(glob_result.gl_pathv[i]);
		}
		// Actualizar la longitud del array
		cl->numwords = newsize;
	}
	globfree(&glob_result);
}

// OPCIONAL 3
void
caseglob(CommandLine *cl)
{

	long long i;

	for (i = 0; i < cl->numwords; i++) {
		setglobbing(cl, cl->words[i], &i);
	}
}

void
parse(CommandLine *cl)
{
	// 1º: $
	caseenv(cl);

	// OPCIONAL 3
	caseglob(cl);

	// 2º: &
	casebg(cl);
	// 3º: > o <
	casered(cl);

	// OPCIONAL 1
	casehere(cl);

	// 4º: |
	casepipes(cl);
	// 5º: =
	caseequal(cl);
}

void
freememory(CommandLine *cl)
{
	long long i;
	long long j;
	int k;

	// liberamos el array de strings 
	for (i = 0; i < cl->numwords; i++) {
		free(cl->words[i]);
	}
	free(cl->words);

	if (cl->status != FINDERROR && cl->status != REDERROR
	    && cl->status != PARSINGERROR) {

		// ha llegado a ejecutar el comando y ha recibido su pid
		free(cl->waitpids);
	}
	// solo se ha creado el de entrada 
	if (cl->statusred == INPUTRED) {
		free(cl->inred);
		if (cl->inredfd >= 0) {
			close(cl->inredfd);
		}
	}
	// solo se ha creado el de salida
	if (cl->statusred == OUTPUTRED) {
		free(cl->outred);
		if (cl->outredfd >= 0) {
			close(cl->outredfd);

		}
	}
	// se han creado los dos
	if (cl->statusred == BOTHRED) {
		free(cl->inred);
		free(cl->outred);

		if (cl->inredfd >= 0) {
			close(cl->inredfd);
		}

		if (cl->outredfd >= 0) {
			close(cl->outredfd);
		}
	}

	if (cl->numpipes > 0) {

		//libera cada subcomando
		for (i = 0; i < cl->numcommands; i++) {

			for (j = 0; j < cl->numsubcommands[i]; j++) {
				free(cl->commands[i][j]);
			}
			// libera comando
			free(cl->commands[i]);
		}

		// libera la estructura completa
		free(cl->commands);
		// libero el array de ints
		free(cl->numsubcommands);
		// libero el array de builtinstatus que me dice si hay y el tipo de built in
		free(cl->statuspipesbt);

		// llega hasta el punto de executecommands
		if (cl->status != FINDERROR && cl->status != REDERROR) {
			for (k = 0; k < cl->numpipes; k++) {
				free(cl->pipesfd[k]);
			}
			free(cl->pipesfd);
		}
	}
	if (cl->heredoc > 0) {
		free(cl->herepipe);
	}
}

void
setspecialchar(CommandLine *cl, char *delim, long long *pos, long long *tpos)
{
	// Si el token actual no está vacío, hay que 
	// acabar la palabra con '\0'
	if (*tpos > 0) {
		cl->words[*pos][*tpos] = '\0';
		// pasamos a la siguiente posición del array en el que insertar los tokens 
		*pos = *pos + 1;
		// se reinicio la posición para el siguiente token
		*tpos = 0;
	}
	// Agrega el delimitador como un token adicional
	strcpy(cl->words[*pos], delim);
	*pos = *pos + 1;
}

void
handlespecialchars(CommandLine *cl, char *word, long long *pos)
{

	long long wpos;
	long long tpos;

	wpos = 0;
	tpos = 0;

	while (word[wpos] != '\0') {
		// Si el caracter actual es '|', '<', '>' o '&, se acaba el token actual 
		// y avanza al siguiente
		switch (word[wpos]) {

		case '|':
			setspecialchar(cl, "|", pos, &tpos);
			break;
		case '>':
			setspecialchar(cl, ">", pos, &tpos);
			break;
		case '<':
			setspecialchar(cl, "<", pos, &tpos);
			break;
		case '&':
			setspecialchar(cl, "&", pos, &tpos);
			break;
		default:
			// Si no es un delimitador, agrega el caracter al token actual
			cl->words[*pos][tpos] = word[wpos];
			// se pasa a la siguiente posición del token actual
			tpos++;
		}
		// Avanza al siguiente caracter de la palabra
		wpos++;
	}
	// Termina el último token si la palabra no termina con algún caracter especial
	if (tpos > 0) {
		cl->words[*pos][tpos] = '\0';
		// incrementa el valor de la posición del array ya que lo
		// que faltaba por acabar conforma otro subtoken
		*pos = *pos + 1;
	}
}

// tokeniza y almacena en un array de strings todos los elementos
void
tokenize(CommandLine *cl, char *line)
{
	long long i;
	char *saveptr;
	char *token;
	char *aux;

	aux = (char *)malloc(sizeof(char) * MaxWord);
	if (aux == NULL) {
		err(EXIT_FAILURE, "Error: dynamic memory cannot be allocated");
	}
	cl->words = (char **)malloc(sizeof(char *) * cl->numwords);
	if (cl->words == NULL) {
		free(aux);
		err(EXIT_FAILURE, "Error: dynamic memory cannot be allocated");
	}
	// inicializamos cada elemento del array
	for (i = 0; i < cl->numwords; i++) {
		cl->words[i] = (char *)malloc(sizeof(char) * MaxWord);
		if (cl->words[i] == NULL) {
			free(aux);
			free(cl->words);
			err(EXIT_FAILURE,
			    "Error: dynamic memory cannot be allocated");
		}
	}

	i = 0;

	token = strtok_r(line, " \t", &saveptr);
	strcpy(aux, token);

	// tokeniza la palabra usando '|', '<', '>', '&'
	handlespecialchars(cl, aux, &i);

	while (token != NULL && i < cl->numwords) {

		token = strtok_r(NULL, " \t", &saveptr);
		strcpy(aux, token);
		// tokeniza la palabra usando '|', '<', '>','&'
		handlespecialchars(cl, aux, &i);
	}
	free(aux);
}

long long
getnumwords(char *line)
{
	long long i;
	long long numwords;
	int inword;

	i = 0;
	numwords = 0;
	// inword se inicializa como falso ya que 
	// indica si nos encontramos dentro de una palabra
	inword = 0;
	while (line[i] != '\0') {

		// si el carácter actual no es ninguno de los que buscamos
		// y no estamos dentro de una palabra, aumentamos el número de palabra
		// y decidimos que nos encontramos dentro de una palabra
		if (line[i] != ' ' && line[i] != '\t' && line[i] != '|'
		    && line[i] != '<' && line[i] != '>' && line[i] != '&') {

			if (!inword) {
				numwords++;
				inword = 1;
			}

		}
		// si el caracter es espacio o tabulador y si ha estado 
		// dentro de una palabra, ponemos que acabamos de salir de ella  
		if (line[i] == ' ' || line[i] == '\t') {
			if (inword) {
				inword = 0;

			}
		}
		// si a lo largo de la linea hay algún caracter que buscamos:
		// inicializamos una palabra nueva 
		if (line[i] == '|' || line[i] == '<' || line[i] == '>'
		    || line[i] == '&') {
			numwords++;
			inword = 0;
		}

		i++;
	}
	return numwords;
}
