#include <string.h>
#include "executor.h"
#include "structs.h"

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

void
executecommands(CommandLine *cl){
    // en este punto -> eje: [[cat, /tmp/a], [wc,-c]]
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
   //recorrer el array de arrays e ir comprobando si hay igual o env
}
