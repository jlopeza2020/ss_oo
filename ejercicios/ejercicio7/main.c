#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <err.h>
#include "stack.h"

enum {
	ZeroArgs,
	NThreads = 1000,
	NPush = 1000,
	NPop = 40,
	ArraySize = 50,
};

struct Valor {
	int v;
	int id;
};
typedef struct Valor Valor;

struct ThreadArgs {
	Stack *stack;
	int id;
};
typedef struct ThreadArgs ThreadArgs;

void
usage(void)
{
	fprintf(stderr, "usage: ./main\n");
	exit(EXIT_FAILURE);
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
			/// no usar err 
			// salir y luego verlo en el main
			//errx(EXIT_FAILURE,
			//     "Error: dynamic memory cannot be allocated");
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

	return NULL;
}

// - los valores de mis threads y de los id van a ir desde 0 a N-1
//              siendo N el número total de Threads
// - También, he decidido inicializar el valor de la cima (postop) a -1 
//              suponiendo que la pila está vacía cuando dicho valor vale -1
int
main(int argc, char *argv[])
{
	pthread_t threads[NThreads];
	ThreadArgs *args;
	Stack *stack;
	Valor *val;
	long long expectedsize;
	long long stacksize;
	int lastid;
	int lastvalue;
	int i;
	int errs;

	errs = 0;

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
		args = (ThreadArgs *)malloc(sizeof(ThreadArgs));
		if (args == NULL) {
			free(args);
			freestack(stack);
			errx(EXIT_FAILURE, "Error: dynamic memory cannot be allocated\n");
		}

		args->stack = stack;
		args->id = i;

		// creo el thread y manejo error si hay fallo
		if (pthread_create
		    (&threads[i], NULL, threadfunction, (void *)args) != 0) {
			//handlerror(stack, args, i, "Error: creating thread");
			//err
		}
	}

	for (i = 0; i < NThreads; i++) {
		// muere el thread y manejo error si hay fallo
		if (pthread_join(threads[i], NULL) != 0) {
			//handlerror(stack, args, i, "Error: joining thread");
			//usar pthread exit
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
				fprintf(stderr,"Popped value: id=%d, v=%d\n", val->id, val->v);
				free(val);
				errs++;
				continue;
			}
			lastid = val->id;
			lastvalue = val->v;
		} else {
			fprintf(stderr,
				"Error: expected size: %lld is not equal to stacksize: %lld\n",expectedsize, stacksize);
			fprintf(stderr,"Popped value: id=%d, v=%d\n", val->id, val->v);
			errs++;
		}

		free(val);
	}

	freestack(stack);

	exit(EXIT_SUCCESS);
}