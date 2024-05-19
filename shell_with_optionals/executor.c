#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <err.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "common.h"
#include "executor.h"


char *builtins[NbuiltIn] = {"cd"};

int
isbuiltin(char *cmd, int *statusbt){

	int i;

	for(i = 0; i < NbuiltIn; i++){

        if(strcmp(cmd, builtins[i]) == 0){
			*statusbt = i;
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
    
	long long j;

	if(cl->numpipes > 0){
		// almacenamos el status de cada comando para facilitarnos a la hora de los pipes
		cl->statuspipesbt = (int *)malloc(sizeof(int) * (cl->numcommands));
		
		if(cl->statuspipesbt == NULL){
			err(EXIT_FAILURE,"Error: dynamic memory cannot be allocated");
		}


		for(j = 0; j < cl->numcommands; j++){
			// inicialmente como no sabemos si existe o no, 
			// lo inicializamos a -1
			cl->statuspipesbt[j] = -1;
			findtypecommand(cl,cl->commands[j][0], &cl->statuspipesbt[j]);
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

/*void 
sethere(int *herepipe){

	char buffer[MaxLine];
	char *newline;
	int c;
	
	close(herepipe[READ]);
	do{
		fgets(buffer, MaxLine, stdin);

		if (buffer[strlen(buffer) - 1] != '\n'){
            fprintf(stderr, "Exceeded path size\n");
            // Limpia el buffer de entrada
            while ((c = getchar()) != '\n' && c != EOF);
            break;
        }

		// Elimina el '\n' de la string
		newline = strrchr(buffer, '\n');

		if (newline != NULL) {
			// donde apunta newline poner un '\0'
			*newline = '\0';
		}
        if (write(herepipe[WRITE], buffer, strlen(buffer)) != strlen(buffer)) {
            err(EXIT_FAILURE, "Error: write error");
        }
		
    }while(strcmp(buffer, "}") != 0);
	close(herepipe[WRITE]);
}*/
// OPCIONAL 1
void 
sethere(int *herepipe) {
    char buffer[MaxLine];
    char *newline;
    int c;
	char *tmpline;
    int totallen = 0;

    close(herepipe[READ]);

    // Usamos un buffer dinámico para almacenar todas las líneas
    tmpline = (char *)malloc(sizeof(char) * MaxLine);
    if (tmpline == NULL) {
		err(EXIT_FAILURE,"Error: dynamic memory cannot be allocated");
    }

    do {
        fgets(buffer, MaxLine, stdin);

        if (buffer[strlen(buffer) - 1] != '\n') {
            fprintf(stderr, "Exceeded path size\n");
            // Limpia el buffer de entrada
            while ((c = getchar()) != '\n' && c != EOF);
            break;
        }

        // Elimina el '\n' de la string
        newline = strrchr(buffer, '\n');
        if (newline != NULL) {
            *newline = '\0';
        }
		tmpline[0] = '\0';

        // Concatena la línea al buffer que almacena todas las líneas
        strcat(tmpline + totallen, buffer);
        totallen += strlen(buffer);
		// si nos pasamos de tamaño máximo, cortamos la str
		if(totallen > MaxLine){
			totallen = MaxLine;
			break;
		}

    } while(strcmp(buffer, "}") != 0);
    // Escribir todo el contenido del buffer en el pipe
    if (write(herepipe[WRITE], tmpline, totallen) != totallen) {
        err(EXIT_FAILURE, "Error: write error");
    }

    close(herepipe[WRITE]);
    free(tmpline); 
}

// numwords, son las palabras que tiene el comando que se le pasa como argumento
pid_t
executecommand(CommandLine *cl, char ***comandline, long long *numwords, long long *pos, int typebuiltin) {

	pid_t pid;
	long long j;
	//int herepipe[2];


	// se aumenta el tamaño de commandline para añadir NULL al final del array de palabras
    (*numwords)++;
    *comandline = (char **)realloc(*comandline, sizeof(char *) * (*numwords + 1));

    if (*comandline == NULL) {
        err(EXIT_FAILURE, "Error: realloc\n");
    }

    (*comandline)[*numwords - 1] = NULL;

	// si hay un built-in pasamos de iteración
	if(typebuiltin >= 0 && *pos < cl->numcommands){
		*pos = *pos +1;
		return 0;
	}

	// HAY OPCIONAL1: es necesario abrir el pipe en el padre
	/*if(cl->heredoc > 0){
		
	
	}*/

    pid = fork();

    switch (pid) {
        case -1:
            err(EXIT_FAILURE, "Error: fork failed");
        case 0:

			// si hay background se fija en el hijo
			setbg(cl);

			// si nos encontramos en la primera iteración y tenemos una redirección
			// de entrada, es necesario duplicar la entrada estándar en el descriptor de esa redirección
			if((cl->statusred == INPUTRED || cl->statusred == BOTHRED) && *pos == 0){

				if (dup2(cl->inredfd, STDIN_FILENO) == -1) {
					err(EXIT_FAILURE,"Error: dup2 failed\n");
				}
				close(cl->inredfd);
			}

			// si nos encontramos en la última iteración y tenemos una redirección
			// de salida, es necesario duplicar la salida estándar en el descriptor de esa redirección
			if((cl->statusred == OUTPUTRED || cl->statusred == BOTHRED) && *pos == cl->numpipes){

				if (dup2(cl->outredfd, STDOUT_FILENO) == -1) {
					err(EXIT_FAILURE,"Error: dup2 failed\n");
				}
				close(cl->outredfd);
			}

			// OPCIONAL 1: dentro del hijo
			if(cl->heredoc > 0){
				close(cl->herepipe[WRITE]);
				if (dup2(cl->herepipe[READ], STDIN_FILENO) == -1) {
					err(EXIT_FAILURE,"Error: dup2 here failed\n");
				}
				close(cl->herepipe[READ]);
			}

			// si hay pipes, hay que hacer más redirecciones
			if(cl->numpipes > 0){

				
				// mientras no estemos en la primera posición, 
				// hay que redireccionar la entrada del pipe anterior 
				if (*pos != 0 && *pos < cl->numcommands) {
            
					if (dup2(cl->pipesfd[*pos - 1][READ], STDIN_FILENO) == -1) {
                		err(EXIT_FAILURE,"Error: dup2 this failed\n");
               	 	}
            	}
				// si no estamos en la última posición, hay que redireccionar el 
				// la salida al pipe actual
            	if (*pos != cl->numpipes && *pos < cl->numcommands) {
                
					if (dup2(cl->pipesfd[*pos][WRITE], STDOUT_FILENO) == -1) {
               	    	err(EXIT_FAILURE,"Error: dup2 that failed\n");
               	 	}
            	}

            	// Cerrar los extremos del pipe en el proceso hijo
            	for (j = 0; j < cl->numpipes; j++) {
                	close(cl->pipesfd[j][READ]);
                	close(cl->pipesfd[j][WRITE]);
            	}
			}	

			execv((*comandline)[0], *comandline);
			errx(EXIT_FAILURE, "Error: command failed\n");
            break;
        default:
    }
	return pid;
}

void 
setwait(pid_t *waitpids, long long childs){

	pid_t wpid;
	int sts;
	long long i;

	wpid = 0;

	while (childs > 0 && wpid != -1) {

		wpid = wait(&sts);

		for(i = 0; i < childs; i++){
			if(wpid == waitpids[i]){
				// si ha acabado el pid del wpid actual, 
				// hay que decrecer el valor de childs
				if(WIFEXITED(sts)){
					childs--;
					break;
				}
			}
		}
	}
}

void
executecommands(CommandLine *cl){
	
	long long j;
	pid_t pid;
	int i;
	long long novalue; 
	pid_t *waitpids;
	long long pos;
	long long wppos;
	long long numwaitprocess;
	
	numwaitprocess = 0;

	// se usa para decir que no hay pipes y para las redirecciones (dup2)
	// es necesario fijar este valor a 0
	novalue = 0;
	pid = 0;

	// se fijan la cantidad de procesos que va a ser necesarios esperar por ellos
	if (cl->numpipes> 0){
		for (j = 0; j < cl->numcommands; j++) {
        	if (cl->numpipes> 0){
				if(cl->statuspipesbt[j] == -1 ){
					numwaitprocess++;
				}
			}
   		}	
	}else{
		if(cl->statusbt == -1){
			numwaitprocess++;
		}
	}
		
	waitpids = (pid_t *)malloc(sizeof(pid_t) * numwaitprocess);
	if(waitpids == NULL){
		err(EXIT_FAILURE,"Error: dynamic memory cannot be allocated");
	}

	//HAY OPCIONAL 1
	if(cl->heredoc > 0){
		cl->herepipe = (int *)malloc(sizeof(int ) * 2);
		if (pipe(cl->herepipe) == -1) {
           	err(EXIT_FAILURE,"Error: pipe failed");
   		}
	}

	// Si hay pipes
	if(cl->numpipes > 0){

		// inicializar descriptores de fichero para pipes
		cl->pipesfd = (int **)malloc(sizeof(int *) * cl->numpipes);
    	if (cl->pipesfd == NULL) {
        	err(EXIT_FAILURE,"Error: dynamic memory cannot be allocated");
    	}
 
    	for (i = 0; i < cl->numpipes; i++) {
        	cl->pipesfd[i] = (int *)malloc(sizeof(int)* 2);
        	if (cl->pipesfd[i] == NULL) {
				free(cl->pipesfd);
            	err(EXIT_FAILURE,"Error: dynamic memory cannot be allocated");
        	}

        	if (pipe(cl->pipesfd[i]) == -1) {
           	 	err(EXIT_FAILURE,"Error: pipe failed");
        	}
   		}	

		// los builtins se encuentran dentro del hijo
		pos = 0;
		wppos = 0;
		for(j = 0; j < cl->numcommands; j++){
					
			pid = executecommand(cl,&cl->commands[j], &cl->numsubcommands[j], &pos, cl->statuspipesbt[j]);
			if(pid != 0){
				waitpids[wppos] = pid;
				wppos++;

			}
			pos++;
		}

		// cerramos los descriptores del padre
		for (i = 0; i < cl->numpipes; i++) {
   	 	    close(cl->pipesfd[i][READ]);
    	    close(cl->pipesfd[i][WRITE]);
    	}


	}else{
		// en este caso el comando es sin pipes
		// significa que el comando actual es un builtin
		if(cl->statusbt >= 0){
			executebuiltin(cl,cl->words, cl->statusbt, cl->numwords);
		}else{
			pid = executecommand(cl,&cl->words, &cl->numwords, &novalue, -1);
			if(pid != 0){
				waitpids[0] = pid;

			}
		}

	}

	if(cl->heredoc > 0){
		sethere(cl->herepipe);
	}

	// aquí se hace el wait: si no he ejecutado 
	// un builtin y no hay que hacer background
	if(!cl->bg){

		setwait(waitpids, numwaitprocess);
	}

	free(waitpids);
}

// Lo estamos abriendo en el proceso padre y habrá que
// cerrarlo en el proceso padre e hijo
void 
openredin(CommandLine *cl){

    cl->inredfd = open(cl->inred, O_RDONLY);
    if ( cl->inredfd == -1) {
        fprintf(stderr,"Error: open this file failed\n");
		cl->status = REDERROR;
    }
}

// Lo estamos abriendo en el proceso padre y habrá que
// cerrarlo en el proceso padre e hijo
void 
openredout(CommandLine *cl){

	cl->outredfd = open(cl->outred, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (cl->outredfd == -1) {
        fprintf(stderr,"Error: open this file failed\n");
		cl->status = REDERROR;
    }
}

void 
handleredirecctions(CommandLine *cl){

	if(cl->statusred == INPUTRED){
		openredin(cl);
	}
	if(cl->statusred == OUTPUTRED){
		openredout(cl);
	}
	if(cl->statusred == BOTHRED){
		openredin(cl);
		openredout(cl);
	}
}

void 
setbg(CommandLine *cl){

	int fdbg;
	// significa que hay background y no ha sido redirigida la entrada
	// a un fichero, entonces hay que redirigirla a /dev/null
	if(cl->inredfd < 0 && cl->bg > 0){

		fdbg = open("/dev/null", O_RDONLY);
    	if (fdbg == -1) {
        	fprintf(stderr,"Error: open /dev/null failed\n");
			return;
    	}
		if (dup2(fdbg, STDIN_FILENO) == -1) {
			err(EXIT_FAILURE,"Error: dup2 failed\n");
		}
		close(fdbg);
	}
}