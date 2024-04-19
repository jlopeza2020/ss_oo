#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <err.h>
#include "stack.h"

void
usage(void)
{
	fprintf(stderr, "usage: ./main\n");
	exit(EXIT_FAILURE);
}

void
handlerror(Stack *st, ThreadArgs **args, int i, char *message)
{
	int j;

	for (j = 0; j < i; j++) {
		// libero todos los ThreadArgs que se han creado hasta el momento
		free(args[j]);
	}
	// también libero la pila creada
	freestack(st);
	errx(EXIT_FAILURE, "%s", message);
}

void *
threadfunction(void *arg)
{

	ThreadArgs *args;
	Stack *stack;
	int id;
	int i;
	Valor *val;
	long long counter;

	args = (ThreadArgs *)arg;

	// como argumentos: pila y id del thread
	stack = args->stack;
	id = args->id;

	for (i = 0; i < NPush; i++) {
		// reservo memoria para la struct Valor (id y v)
		val = (Valor *)malloc(sizeof(Valor));
		if (val == NULL) {
			free(args);
			errx(EXIT_FAILURE,
			     "Error: dynamic memory cannot be allocated");
		}
		// asigno valores a la estructura Valor 
		val->v = i;
		val->id = id;
		push(stack, val);
	}

	counter = 0;
	for (i = 0; i < NPop; i++) {
		val = (Valor *)pop(stack);
		// significa si el valor no es NULL
		if (val) {
			// si id del pop (val) y el id del thread son diferentes:
			// aumenta el contador
			if (val->id != id) {
				counter++;
			}
			// libero el valor de la struct Valor porque ya no lo necesito
			free(val);
		}
	}
	if (counter != 0) {
		printf("Id %d: there are %lld incorrect pops\n", id, counter);
	}
	// libero la memoria de la struct ThreadArgs
	free(args);

	return NULL;		// ???
}

// - los valores de mis threads y de los id van a ir desde 0 a N-1
//              siendo N el número total de Threads
// - También, he decidido inicializar el valor de la cima (postop) a -1 
//              suponiendo que la pila está vacía cuando dicho valor vale -1
int
main(int argc, char *argv[])
{

	pthread_t threads[NThreads];
	ThreadArgs *args[NThreads];
	Stack *stack;
	Valor *val;
	long long expectedsize;
	long long stacksize;
	int lastid;
	int lastvalue;
	int i;

	argc--;
	argv++;

	if (argc != ZeroArgs) {
		usage();
	}
	// Crea y asigna memoria para la pila
	stack = createstack(ArraySize);
	if (stack == NULL) {
		errx(EXIT_FAILURE, "Error creating stack");
	}

	for (i = 0; i < NThreads; i++) {
		// Asigna memoria para cada argumentos del thread
		args[i] = (ThreadArgs *)malloc(sizeof(ThreadArgs));
		if (args[i] == NULL) {
			handlerror(stack, args, i,
				   "Error: dynamic memory cannot be allocated");
		}

		args[i]->stack = stack;
		args[i]->id = i;

		// creo el thread y manejo error si hay fallo
		if (pthread_create
		    (&threads[i], NULL, threadfunction, (void *)args[i]) != 0) {
			handlerror(stack, args, i, "Error: creating thread");
		}
	}

	for (i = 0; i < NThreads; i++) {
		// muere el thread y manejo error si hay fallo
		if (pthread_join(threads[i], NULL) != 0) {
			handlerror(stack, args, i, "Error: joining thread");
		}
	}

	expectedsize = NThreads * NPush - NThreads * NPop;
	stacksize = size(stack);

	lastid = -1;
	lastvalue = -1;
	while (!isempty(stack)) {
		val = (Valor *)pop(stack);

		if (expectedsize == stacksize) {
			// comprueba que valores con el mismo id salen en orden decreciente. 
			if (val->id == lastid && val->v > lastvalue) {
				fprintf(stderr,
					"Error: Values with the same id are not in decreasing order\n");
				free(val);
				continue;
			}
			lastid = val->id;
			lastvalue = val->v;
			printf("Popped value: id=%d, v=%d\n", val->id, val->v);
		} else {
			fprintf(stderr,
				"Error: Expected values are not the same as in the stack\n");
		}

		free(val);
	}

	freestack(stack);

	exit(EXIT_SUCCESS);
}
