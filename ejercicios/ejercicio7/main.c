#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <err.h>
#include "stack.h"

// PREGUNTAR y si no se puede hacer
// ¿cómo pasarle el id al thread y la pila?
Stack *stack;

void
usage(void)
{
	fprintf(stderr, "usage: ./main\n");
	exit(EXIT_FAILURE);
}

// se puede hacer pública
static void 
destroystack(void){
    free(stack->array);
    pthread_mutex_destroy(&stack->mutex);
    free(stack);
}

// las operaciones que hace el thread
void *
threadfunction(void *arg){

    int id;
    Valor *val;

    id = *((int*)arg);

    for (int i = 0; i < NPush; i++) {
        val = (Valor*)malloc(sizeof(Valor));
        // comprobar si malloc ha ido bien o no 
        if (val == NULL) {
		    errx(EXIT_FAILURE, "Error: dynamic memory cannot be allocated");
	    }

        val->v = i;
        val->id = id;
        push(stack, val);
    }

    for (int i = 0; i < NPop; i++) {

        val = (Valor*)pop(stack);

        if(val){
            fprintf(stderr,"My popy4: id=%d, v=%d\n", val->id, val->v);
            free(val);
        }
        // aquí llegarían cuando sean NULL
    }

    return NULL;
}

int 
main(int argc, char *argv[]) {
    
    //Stack stack;
    pthread_t threads[NThreads];
    /// hacerlo con malloc
    // hacer una struct que se le inserte la pila
    // no poner la pila como global
    int ids[NThreads];
    Valor *val;
    int i;

    argc--;
	argv++;

	if (argc != ZeroArgs) {
		usage();
	}

    // stack es global ??? 
    stack = createstack(ArraySize);

    for (i = 0; i < NThreads; i++) {
        ids[i] = i;

        if(pthread_create(&threads[i], NULL, threadfunction,(void*)&ids[i]) != 0){
            destroystack();
            errx(EXIT_FAILURE, "Error: creating thread");
        }
    }

    for(i = 0; i < NThreads; i++){
        if(pthread_join(threads[i], NULL) != 0){
            destroystack();
            errx(EXIT_FAILURE, "Error: joining thread");
        }
    }

    // si ocurre el caso de más push que pop, hay que tener
    // en cuenta hacer pop hasta que se igualen los valores
    // cuando se destruya la pila
    printf(" Hay %lld elementos\n", size(stack));

    while(!isemptystack(stack)){
        val = (Valor*)pop(stack);

        if(val){
            fprintf(stderr,"Remaining: id=%d, v=%d\n", val->id, val->v);
            free(val);
        }
    }

    destroystack();

    exit(EXIT_SUCCESS);
}
