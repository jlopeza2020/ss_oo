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

void *
threadfunction(void *arg){

    int id;
    Valor *val;
    ThreadArgs *args = (ThreadArgs *)arg;

    Stack *stack = args->stack; 
    id = args->id; 


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

        }
        // aquí llegarían cuando sean NULL
    }
    free(args);
    return NULL;
}

int 
main(int argc, char *argv[]){
    
    pthread_t threads[NThreads];
    ThreadArgs *args[NThreads];
    Stack *stack;
    Valor *val;
    int i;
    int j;

    argc--;
	argv++;

	if (argc != ZeroArgs) {
		usage();
	}

    // Crear y asignar memoria para la pila
    stack = createstack(ArraySize);
    if (stack == NULL) {
        errx(EXIT_FAILURE, "Error creating stack");
    }

    for (i = 0; i < NThreads; i++) {
        
        args[i] = (ThreadArgs *)malloc(sizeof(ThreadArgs));
        
        if (args[i] == NULL) {
            perror("Error allocating memory for thread data");
            // Liberar memoria asignada previamente
            for (j = 0; j < i; j++) {
                free(args[j]);
            }
        }
         

        args[i]->stack = stack;
        args[i]->id = i;

        //ids[i] = i;

        if(pthread_create(&threads[i], NULL, threadfunction,(void*)args[i]) != 0){
            //free(args);
            for (j = 0; j < i; j++) {
                free(args[j]);
            }
            freestack(stack);
            errx(EXIT_FAILURE, "Error: creating thread");
        }
    }

    for(i = 0; i < NThreads; i++){
        if(pthread_join(threads[i], NULL) != 0){
            for (j = 0; j < i; j++) {
                free(args[j]);
            }
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
    freestack(stack); // REVISAR

    exit(EXIT_SUCCESS);
}
