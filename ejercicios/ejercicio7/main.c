#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <err.h>
#include "stack.h"

// PREGUNTAR y si no se puede hacer
// ¿cómo pasarle el id al thread y la pila?
//Stack *stack;

void
usage(void)
{
	fprintf(stderr, "usage: ./main\n");
	exit(EXIT_FAILURE);
}

// se puede hacer pública
/*static void 
destroystack(void){
    free(stack->array);
    pthread_mutex_destroy(&stack->mutex);
    free(stack);
}*/

// las operaciones que hace el thread
void *
threadfunction(void *arg){

    int id;
    Valor *val;

    //id = *((int*)arg);
    ThreadArgs *args = (ThreadArgs *)arg; // Cast arg to ThreadArgs pointer

    Stack *stack = args->stack; // Access the stack pointer from ThreadArgs
    id = args->id; // Access the thread ID from ThreadArgs


    for (int i = 0; i < NPush; i++) {
        val = (Valor*)malloc(sizeof(Valor));
        // comprobar si malloc ha ido bien o no 
        if (val == NULL) {
		    free(args);
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
            // free(args);

        }
        // aquí llegarían cuando sean NULL
    }
    free(args);
    return NULL;
}

int 
main(int argc, char *argv[]) {
    
    pthread_t threads[NThreads];
    // Hacer un malloc de un array de ThreadArgs
    // se le pasa a todos la misma pila y a cada uno un identificador
    // dentro del thread es quien hace free de todo eso
    ThreadArgs *args;
    Stack *stack;
    /// hacerlo con malloc
    // hacer una struct que se le inserte la pila
    // no poner la pila como global
    //int ids[NThreads];
    Valor *val;
    int i;

    argc--;
	argv++;

	if (argc != ZeroArgs) {
		usage();
	}

    stack = createstack(ArraySize);
    if (stack == NULL) {
        errx(EXIT_FAILURE, "Error creating stack");
    }

    args = (ThreadArgs *)malloc(sizeof(ThreadArgs)*NThreads);

    if (args == NULL) {
	    errx(EXIT_FAILURE, "Error: dynamic memory cannot be allocated");
	}


    for (i = 0; i < NThreads; i++) {
        
        //args[i] = (ThreadArgs *)malloc(sizeof(ThreadArgs));
        
        /*if (args[i] == NULL) {
		    errx(EXIT_FAILURE, "Error: dynamic memory cannot be allocated");
	    }*/

        args[i].stack = stack;
        args[i].id = i;

        //ids[i] = i;

        if(pthread_create(&threads[i], NULL, threadfunction,(void*)&args[i]) != 0){
            free(args);
            freestack(stack);
            errx(EXIT_FAILURE, "Error: creating thread");
        }
    }

    for(i = 0; i < NThreads; i++){
        if(pthread_join(threads[i], NULL) != 0){
            free(args);
            freestack(stack);
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

    //free(args);
    freestack(stack);
    //free(args);

    exit(EXIT_SUCCESS);
}
