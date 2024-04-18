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
handlerror(Stack *st, ThreadArgs **args, int i, char *message){

    int j;

    for (j = 0; j < i; j++) {
        free(args[j]);
    }
    // también libero la pila creada
    freestack(st);
    errx(EXIT_FAILURE, "%s", message);
        

}
void *
threadfunction(void *arg){

    ThreadArgs *args;
    Stack *stack;
    int id;
    Valor *val;
    long long counter;

    counter = 0;

    args = (ThreadArgs *)arg;

    // extraigo de los argumentos el puntero a la pila y 
    // el identificador del thread 
    stack = args->stack; 
    id = args->id; 

    // hago N veces push
    for (int i = 0; i < NPush; i++) {
        // reservo memoria para la struct Valor (id y v)
        val = (Valor*)malloc(sizeof(Valor));
        if (val == NULL) {
		    free(args);
            errx(EXIT_FAILURE, "Error: dynamic memory cannot be allocated");
	    }

        val->v = i;
        val->id = id;
        push(stack, val);
    }

    // hará N veces pop
    for (int i = 0; i < NPop; i++) {
        val = (Valor*)pop(stack);
        // significa si el valor no es NULL
        if(val){
            // si el id obtenido del pop (val) y el id del thread 
            // son diferentes: se contará e imprimirá el contador por la salida
            if(val->id != id){
                counter++;
            }
            //fprintf(stderr,"My popy4: id=%d, v=%d\n", val->id, val->v);
            // libero el valor de la struct Valor porque ya no lo necesito
            free(val);
        }
    }

    printf("In id: %d, there is/are %lld incorrect pop/s", id, counter);
    // libero la memoria de la struct ThreadArgs
    free(args);

    return NULL; // ???
}

int 
main(int argc, char *argv[]){
    
    pthread_t threads[NThreads];
    ThreadArgs *args[NThreads];
    Stack *stack;
    Valor *val;
    int i;
    int j;
    long long expectedsize;

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
        
        // Asigna memoria para cada ThreadArgs struct
        args[i] = (ThreadArgs *)malloc(sizeof(ThreadArgs));
        
        // si no se ha conseguido asignar
        if (args[i] == NULL) {
            // libero todos los ThreadArgs que se han creado hasta el momento
            handlerror(stack, args, i, "Error: dynamic memory cannot be allocated");
            
            /*for (j = 0; j < i; j++) {
                free(args[j]);
            }
            // también libero la pila creada
            freestack(stack);
            errx(EXIT_FAILURE, "Error: dynamic memory cannot be allocated");*/
        }
        // asigna a ThreadArgs: 
        //  - la misma pila para todos
        //  - Asigno como id el valor de i
        args[i]->stack = stack;
        args[i]->id = i;

        // creo el thread que recibirá como argumento la struct ThreadArgs creada
        if(pthread_create(&threads[i], NULL, threadfunction,(void*)args[i]) != 0){
            // si no se ha conseguido: libero todos 
            // los ThreadArgs que se han creado hasta el momento
            handlerror(stack, args, i, "Error: creating thread");

            /*for (j = 0; j < i; j++) {
                free(args[j]);
            }
            freestack(stack);
            errx(EXIT_FAILURE, "Error: creating thread");*/
        }
    }

    // Espera a que mueran los threads creados 
    // y libera la memoria si hay error 
    for(i = 0; i < NThreads; i++){
        if(pthread_join(threads[i], NULL) != 0){
            handlerror(stack, args, i, "Error: joining thread");
            /*for (j = 0; j < i; j++) {
                free(args[j]);
            }
            freestack(stack);
            errx(EXIT_FAILURE, "Error: joining thread");*/
        }
    }

    printf(" Hay %lld elementos\n", size(stack));

    expectedsize = NThreads*NPush - NThreads*NPop;
    printf(" Se espera %lld e\n", expectedsize);


    // mirar que salgan en el orden correcto hasta que se vacíe la pila 
    //if()
    while(!isempty(stack)){
        val = (Valor*)pop(stack);

        if(val){
            fprintf(stderr,"Remaining: id=%d, v=%d\n", val->id, val->v);
            free(val);
        }
    }

    freestack(stack);

    exit(EXIT_SUCCESS);
}
