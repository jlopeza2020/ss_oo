#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <err.h>
#include "common.h"
#include "executor.h"


char *builtins[NbuiltIn] = {"cd"};

// 	- builtin
//  - fichero ejecutable del dir de trabajo 
//  - fichero ejecutable de alguno de la variable PATH
int
isbuiltin(char *cmd){

	int i;

	for(i = 0; i < NbuiltIn; i++){

        if(strcmp(cmd, builtins[i]) == 0){
            return 1;
        }
    }
	return 0;
}

char *
getcompletepath(char *path, char *dname)
{

	char *fullpath;
	ssize_t lenpath;
	ssize_t lenname;
	ssize_t lenfull;

	// obtenemos la longitud del path
	lenpath = strlen(path);
	// obtenemos la longitud del nombre de entrada de directorio 
	lenname = strlen(dname);

	// path + '/' + nombre de entrada directorio + '\0' 
	lenfull = lenpath + lenname + 2;

	// comprobar que el malloc no es muy grande 
	if (lenfull > MaxWord) {
		fprintf(stderr,"Error: Invalid path size");
        return NULL;
	}
	fullpath = (char *)malloc(sizeof(char) * (lenfull));
	if (fullpath == NULL) {
		err(EXIT_FAILURE,"Error: dynamic memory cannot be allocated");
        return NULL;
	}

	strncpy(fullpath, path, lenfull);
	// después del path añadir '/'
	fullpath[lenpath] = '/';
	// strncpy no añade '\0' a final de línea
	fullpath[lenfull - 1] = '\0';
	strncpy(fullpath + lenpath + 1, dname, lenfull - lenpath - 1);
	fullpath[lenfull - 1] = '\0';

	return fullpath;
}

int 
ispwd(char *cmd){

	char *fullpath;
	char pwdpath[MaxWord];
	char *value;

	//fprintf(stderr, "valores: %s\n", getenv("PWD"));

	value = getenv("PWD");

	if (value != NULL) {
		//fprintf(stderr, " value: %s \n", value);
		strcpy(pwdpath, value);
		// sustituir el valor de value en str para hacer
		// la ejecución de los comandos 
	}
	// crear una función 

	fullpath = getcompletepath(pwdpath, cmd);
    if(fullpath != NULL){

        //fprintf(stderr, "la línea resultante es %s\n", fullpath);

        // si el valor es 0: el fichero existe y es ejecutable
        if (access(fullpath, X_OK) == 0){
            //fprintf(stderr, "Error: this file exists\n");
			// almacenar en la estrutura el path completo
			free(fullpath);
			return 1;
		}

	}
	// cuando se solucione hay que meter el path completo en la estructura
	// de datos commandline
	free(fullpath);
	
	return 0;
}
char **
tokenpath(char *path, long long times){

	long long i;
	char *saveptr;
	char *token;
	char **tokens;

	tokens =
	    (char **)malloc(sizeof(char *) * (times+1));
	if (tokens == NULL) {
		err(EXIT_FAILURE,"Error: dynamic memory cannot be allocated");
	}
	// inicializamos cada elemento del array
	for (i = 0; i < (times + 1); i++) {
		tokens[i] = (char *)malloc(sizeof(char) * MaxWord);
		if (tokens[i] == NULL) {
			err(EXIT_FAILURE,"Error: dynamic memory cannot be allocated");
		}
	}

	i = 0;

	token = strtok_r(path, ":" , &saveptr);

	while (token != NULL && i < (times+ 1)) {
		strcpy(tokens[i], token);
		i++;
		token = strtok_r(NULL, ":", &saveptr);
	}

	return tokens;
}


long long
getnumpathtokens(char *line)
{
	long long i;
	long long numtokens;

	i = 0;
	numtokens = 0;

	while (line[i] != '\0') {

		if (line[i] == ':') {
			numtokens++;
		}

		i++;
	}
	return numtokens;
}

int
ispath(char *cmd){

	char *fullpath;
	char pathpath[MaxWord];
	char *value;
	long long numtokens;
	long long i;
	char **tokens;
	int iscorrect;

	iscorrect = 0;

	//fprintf(stderr, "valores: %s\n", getenv("PWD"));

	value = getenv("PATH");

	// tokenizarlo y hacerlo esto 12 veces que hay 11 ':'

	if (value != NULL) {
		//fprintf(stderr, " value: %s \n", value);
		strcpy(pathpath, value);
		// sustituir el valor de value en str para hacer
		// la ejecución de los comandos 
	}

	numtokens = getnumpathtokens(pathpath);
	tokens = tokenpath(pathpath, numtokens);


	
	for(i = 0; i < (numtokens + 1); i++){

		fullpath = getcompletepath(tokens[i], cmd);
    	if(fullpath != NULL){

        	fprintf(stderr, "la línea resultante es %s\n", fullpath);

        	// si el valor es 0: el fichero existe y es ejecutable
        	if (access(fullpath, X_OK) == 0){
            	fprintf(stderr, "Winner: %s\n", fullpath);
				iscorrect = 1;
			}

		}
		free(fullpath);
	}

	for(i = 0; i < (numtokens + 1); i++){
		fprintf(stderr, "token: %lld, %s\n", i,tokens[i]);
		free(tokens[i]);
	}

	free(tokens);
	//free(fullpath);


	// crear una función 

	/*fullpath = getcompletepath(pathpath, cmd);
    if(fullpath != NULL){

        fprintf(stderr, "la línea resultante es %s\n", fullpath);

        // si el valor es 0: el fichero existe y es ejecutable
        if (access(fullpath, X_OK) == 0){
            //fprintf(stderr, "Error: this file exists\n");
			return 1;
		}

	}
	// cuando se solucione hay que meter el path completo en la estructura
	// de datos commandline
	free(fullpath);*/

	return iscorrect;

}

// setbuitlin
// hacer swicth case e incluir todas las funciones
void  
findtypecommand(CommandLine *cl, char *cmd){

	if(isbuiltin(cmd)){
		fprintf(stderr,"soy built in\n");
		//setbuiltin(cmd)
	}
	if (ispwd(cmd)){
		fprintf(stderr,"soy pwd\n");

		//setpwd()
	}
	if(ispath(cmd)){
		fprintf(stderr,"soy path\n");

		//setpath()
	}

	//if(!isbuiltin(cmd) && !ispwd(cmd) && !ispath(cmd)){
	if(!isbuiltin(cmd) && !ispwd(cmd)){

		cl->status = FINDERROR;
	}		
}

void
findcommands(CommandLine *cl){
    
	
	// Apartir de aquí podemos hacer:
	// 	mirar el primer elemento de cada array y ver si se trata de: 
		// 	- builtin
		//  - fichero ejecutable del dir de trabajo 
		//  - fichero ejecutable de alguno de la variable PATH

	// Para ello distinguir si se trata de un pipeline o no

	long long j;
	//long long k;

	if(cl->numpipes > 0){

		for(j = 0; j < cl->numcommands; j++){
			//for(k = 0; k < cl->numsubcommands[j]; k++){
			findtypecommand(cl,cl->commands[j][0]);
				//fprintf(stderr,"soy el subcommando: %s\n", cl.commands[j][0]);		
			//}

		}
	}else{

		findtypecommand(cl,cl->words[0]);
		//findtypecommand(c)
	}
}
