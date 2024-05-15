#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <err.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "common.h"
#include "executor.h"


char *builtins[NbuiltIn] = {"cd"};

int
isbuiltin(char *cmd, int *statusbt){

	int i;

	for(i = 0; i < NbuiltIn; i++){

        if(strcmp(cmd, builtins[i]) == 0){
			*statusbt = i;
			fprintf(stderr, "valor de statusbt: %d\n", *statusbt);
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
        //return NULL;
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
setpwd(char *cmd){

	char *fullpath;
	char pwdpath[MaxWord];
	char *value;
	int iscorrect;

	iscorrect = 0;

	value = getenv("PWD");

	if (value != NULL) {
		strcpy(pwdpath, value);
	}
	// crear una función 

	fullpath = getcompletepath(pwdpath, cmd);
    if(fullpath != NULL){

        // si el valor es 0: el fichero existe y es ejecutable
        if (access(fullpath, X_OK) == 0){
			// almacenar en la estrutura el path completo
			strcpy(cmd, fullpath);
			iscorrect = 1;
		}

	}
	free(fullpath);
	
	return iscorrect;
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
setpath(char *cmd){

	char *fullpath;
	char pathpath[MaxWord];
	char *value;
	long long numtokens;
	long long i;
	char **tokens;
	int iscorrect;

	iscorrect = 0;

	value = getenv("PATH");

	if (value != NULL) {
		strcpy(pathpath, value);
	}

	numtokens = getnumpathtokens(pathpath);
	tokens = tokenpath(pathpath, numtokens);


	for(i = 0; i < (numtokens + 1); i++){

		fullpath = getcompletepath(tokens[i], cmd);
    	if(fullpath != NULL){

        	// si el valor es 0: el fichero existe y es ejecutable
        	if (access(fullpath, X_OK) == 0){
            	// ALMACENARNOS ESE VALOR
				strcpy(cmd, fullpath);
				iscorrect = 1;
			}

		}
		free(fullpath);
	}

	for(i = 0; i < (numtokens + 1); i++){
		free(tokens[i]);
	}

	free(tokens);

	return iscorrect;

}

void  
findtypecommand(CommandLine *cl, char *cmd, int *statusbt){

	int isbt; 
	int ispwd;
	int ispath;

	// no ejecutamos el built in porque así vemos si hay errores o no
	isbt = isbuiltin(cmd, statusbt);
	ispwd = setpwd(cmd);
	ispath = setpath(cmd);

	if(!isbt && !ispwd && !ispath){

		cl->status = FINDERROR;
	}		
}

// se mira si es un builtin, pertenece a $PWD o a $PATH
// si pertenece a $PWD o $PATH cambia el path 
void
findcommands(CommandLine *cl){
    
	// Para ello distinguir si se trata de un pipeline o no

	long long j;

	if(cl->numpipes > 0){
		// inicializamos memoria para cl->statusbt;
		cl->statuspipesbt = (int *)malloc(sizeof(int) * (cl->numcommands));


		for(j = 0; j < cl->numcommands; j++){
			// inicialmente como no sabemos si existe o no, 
			// lo inicializamos a -1
			cl->statuspipesbt[j] = -1;
			findtypecommand(cl,cl->commands[j][0], &cl->statuspipesbt[j]);
			// Así evitamos iteraciones innecesarias REVISAR DE ACUERDO AL ENUNCIADO
		}
	}else{

		findtypecommand(cl,cl->words[0], &cl->statusbt);
	}
}


void 
executecd(char **cl, long long numwords){
	
	char cdvar[MaxWord];
	if(numwords == 1){
		strcpy(cdvar, getenv("HOME"));
	}else{
		if(strlen(cl[1]) > MaxWord){
			fprintf(stderr,"file or directory too long\n");
			// fijar el valor de cl->status para lo de ifok
			return;
		}
		strcpy(cdvar, cl[1]);
	}

	if(chdir(cdvar) != 0){
		fprintf(stderr,"cd: %s not such file or directory\n", cdvar);
		// fijar el valor de cl->status para lo de ifok
	}
}

void
executebuiltin(CommandLine *cl, char **comandline, int type, long long numwords){

	switch (type) {
	case cd:
		if(numwords > 2){
			fprintf(stderr,"cd: too many arguments\n");
			// añadir el error en cl status para lo de ifok
			return;
		}
		executecd(comandline, numwords);
		break;
	default:
	}
	// AQUÍ AÑADIR EL RESTO DE BUILTINS
}

void executecommand(CommandLine *cl, char ***comandline, long long *numwords) {
    // Revisar lo de fork exec...
    // Mirar redirecciones
    // Mirar lo de bg

	pid_t pid;
	pid_t waitpid;
    int sts;

	// se aumenta para añadir NULL al final del array de palabras
    (*numwords)++;
    *comandline = (char **)realloc(*comandline, sizeof(char *) * (*numwords + 1));

    if (*comandline == NULL) {
        err(EXIT_FAILURE, "Error: realloc\n");
    }

    (*comandline)[*numwords - 1] = NULL;

    pid = fork();

    switch (pid) {
        case -1:
            err(EXIT_FAILURE, "Error: fork failed");
            // Actualizar cl->status
            // exit(EXIT_FAILURE);
        case 0:

			// duplicar si hay entradas
            execv((*comandline)[0], *comandline);
			fprintf(stderr, "Error: command failed\n");
            break;
        default:

		
		// esto tiene que ir fuera y devolver el valor de status
		while ((waitpid = wait(&sts)) != -1) {
			if (pid == waitpid) {
				if (!WIFEXITED(sts)) {
					// actualizar el valor de cl.status
					// liberar memoria y salir
					//errx(EXIT_FAILURE, "Error: wait failed\n");
				}
			}
		}
    }
}

void
executecommands(CommandLine *cl){
	
	long long j;

	// trato el fichero de entrada
	
	if(cl->numpipes > 0){

		for(j = 0; j < cl->numcommands; j++){
			if(cl->statuspipesbt[j] >= 0){
				executebuiltin(cl,cl->commands[j], cl->statuspipesbt[j], cl->numsubcommands[j]);
			}else{
				
				executecommand(cl,&cl->commands[j], &cl->numsubcommands[j]);
			}
			// fijar redirecciones y bg 
		}


		// hacer el wait aquí si no hay bg

	}else{

		if(cl->statusbt >= 0){
			executebuiltin(cl,cl->words, cl->statusbt, cl->numwords);
		}else{
			executecommand(cl,&cl->words, &cl->numwords);
		}

		// fijar redirecciones y bg 
		// hacer el wait aquí


	}

	// trato el fichero de salida 
	// probar a hacer el wait aquí  si no hay &


}
