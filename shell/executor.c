#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "common.h"
#include "executor.h"

// comprueba que hay uno y sólo al principio de la palabra
/*int
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
}*/

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

void 
setequal(char *str){

	char *token;
	char *saveptr;
	char name[MaxWord];
	char value[MaxWord];

	token = strtok_r(str, "=", &saveptr);
	strcpy(name, token);
	token = strtok_r(NULL, "=", &saveptr);
	strcpy(value, token);


	fprintf(stderr, " name: %s  = value: %s\n", name, value);

	if (setenv(name, value, 0) != 0){
		fprintf(stderr,"Env value could not be set\n");
	}

}

/*void
setenvvar(char *str){

	char *value;

	//aritmética de punteros
	str = str+1;
	fprintf(stderr, " name: %s \n", str);

	value = getenv(str);
	if (value != NULL) {
		fprintf(stderr, " value: %s \n", value);
		// sustituir el valor de value en str para hacer
		// la ejecución de los comandos 
	}else{
		fprintf(stderr, "error: var %s does not exist\n", str);
		// acabar la ejecución aquí
	}

}*/

// sustituciones de $ y = 
/*void
configvariables(CommandLine *cl){
	int i;
	int j;

	if(cl->numpipes == 0){
		fprintf(stderr, "NO hay pipes\n");

		for(i = 0; i < cl->numwords; i++){
			fprintf(stderr, "%s ", cl->words[i]);
			if(isequal(cl->words[i])){
				fprintf(stderr, "tengo un igual\n");
				// hacer la sustitución correspondiente
				setequal(cl->words[i]);
			}
			// ir comprobando si cada palabra son variables de entorno 
			if(isenv(cl->words[i])){
				fprintf(stderr, "soy una variable de entorno\n");
				// hacer la sustitución correspondiente
				setenvvar(cl->words[i]);
			}
		}
		fprintf(stderr, "\n");
	}else{
		//existen pipes 
		for(i = 0; i < cl->numcommands; i++){
			for(j = 0; j < cl->numsubcommands[i]; j++){
			fprintf(stderr, "%s ", cl->commands[i][j]);

			if(isequal(cl->commands[i][j])){
				fprintf(stderr, "tengo un igual\n");
				// hacer la sustitución correspondiente
				setequal(cl->commands[i][j]);
			}
			// ir comprobando si cada palabra son variables de entorno 
			if(isenv(cl->commands[i][j])){
				fprintf(stderr, "soy una variable de entorno\n");
				// hacer la sustitución correspondiente
				setenvvar(cl->commands[i][j]);
			}
			}
			fprintf(stderr, "\n");
		}
	}
}*/

void
executecommands(CommandLine *cl){
    
	//configvariables(cl);

	// Apartir de aquí podemos hacer:
	// 	mirar el primer elemento de cada array y ver si se trata de: 
		// 	- builtin
		//  - fichero ejecutable del dir de trabajo 
		//  - fichero ejecutable de alguno de la variable PATH


}
