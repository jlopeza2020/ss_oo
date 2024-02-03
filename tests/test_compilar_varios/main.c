#include <stdlib.h>		/* Instrucciones para el preprocesador */
#include <stdio.h>
# include "dist.h"
/* Comentario */

// int dist(int, int);

int
main(int argc, char *argv[])
{				/* Definici´on de funci´on.
				   Punto de entrada. */
	/* Inicio de bloque */
	printf("La distancia 8 y 6 es: %d\n", dist(8, 6));	/* Sentencia, llamada a funci´on */
	exit(EXIT_SUCCESS);	/* Sentencia, llamada a funci´on */
}				/* Fin de bloque */
