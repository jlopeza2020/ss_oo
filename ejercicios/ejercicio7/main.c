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

static void 
destroystack(Stack *st){
    free(st->array);
    // preguntar si hay que reservar la propia estructura
}


Stack stack;
//int incorrect_pops = 0;

/*void* thread_function(void *arg) {
    int id = *((int*)arg);

    for (int i = 0; i < NPush; i++) {
        Valor *val = (Valor*)malloc(sizeof(Valor));
        val->v = i;
        val->id = id;
        push(stack, val);
    }

    for (int i = 0; i < NPop; i++) {
        Valor *val = (Valor*)pop(stack);
        if (val && val->id != id) {
            incorrect_pops++;
            printf("Incorrect pop: id=%d, v=%d\n", val->id, val->v);
        }
        free(val);
    }

    return NULL;
}*/

// las operaciones que hace el thread
void *
threadfunction(void *arg){

    int id;
    Valor *val; // se declara así porque la pop devuelve (void *)

    id = *((int*)arg);

    for (int i = 0; i < NPush; i++) {
        val = (Valor*)malloc(sizeof(Valor));

        val->v = i;
        val->id = id;
        push(&stack, val);
    }

    for (int i = 0; i < NPop; i++) {
        //val = (Valor*)pop(&stack); PREGUNTAR !!!
        val = pop(&stack);

        if(val){
            fprintf(stderr,"My pop: id=%d, v=%d\n", val->id, val->v);
            free(val);
        }
        // aquí llegarían cuando sean NULL
    }

    return NULL;


}

int 
main(int argc, char *argv[]) {
    
    // tien que ser  global??
    //Stack stack;
    //Valor *val; // se declara así porque la pop devuelve (void *)
    pthread_t threads[NThreads];
    int ids[NThreads];

    int i;

    argc--;
	argv++;

	if (argc != ZeroArgs) {
		usage();
	}

    createstack(&stack,ArraySize);

    for (i = 0; i < NThreads; i++) {
        ids[i] = i;
        //if(pthread_create(&threads[i], NULL, threadfuntion, (void*)&thread_args[i]);

        if(pthread_create(&threads[i], NULL, threadfunction,(void*)&ids[i]) != 0){
            destroystack(&stack);
            errx(EXIT_FAILURE, "Error: creating thread");
        }
    }

    for(i = 0; i < NThreads; i++){
        if(pthread_join(threads[i], NULL) != 0){
            destroystack(&stack);
            errx(EXIT_FAILURE, "Error: joining thread");
        }
    }

    //// USADO PARA EJEMPLOS
    /*for (int i = 0; i < 5; i++) {
        val = (Valor*)malloc(sizeof(Valor));

        val->v = i;
        val->id = 3;
        push(&stack, val);
    }

    for (int i = 0; i < 10; i++) {
        //val = (Valor*)pop(&stack); PREGUNTAR !!!
        val = pop(&stack);

    
        if(val){
            fprintf(stderr,"My pop: id=%d, v=%d\n", val->id, val->v);
            free(val);
        }
        // aquí llegarían cuando sean NULL
    }*/

    // si ocurre el caso de más push que pop, hay que tener
    // en cuenta hacer pop hasta que se igualen los valores
    // cuando se destruya la pila

    //// USADO PARA EJEMPLOS 

    destroystack(&stack);


    //pthread_t threads[NThreads];
    //int thread_args[NThreads];

    /*for (int i = 0; i < NThreads; i++) {
        thread_args[i] = i;
        pthread_create(&threads[i], NULL, thread_function, (void*)&thread_args[i]);
    }*/

    /*for (int i = 0; i < NThreads; i++) {
        pthread_join(threads[i], NULL);
    }*/

    //printf("Incorrect pops: %d\n", incorrect_pops);
    //printf("Remaining items in stack: %d\n", size(stack));

    /*while (!is_empty(stack)) {
        Valor *val = (Valor*)pop(stack);
        printf("Remaining item: id=%d, v=%d\n", val->id, val->v);
        free(val);
    }*/

    //destroy_stack(stack);
    exit(EXIT_SUCCESS);
}
