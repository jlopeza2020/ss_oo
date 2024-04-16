#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
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
// no puede haber variables globales 
//Stack *stack;
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

int main(int argc, char *argv[]) {
    
    Stack stack;
    //Valor val2;
    Valor *val;
    argc--;
	argv++;

	if (argc != ZeroArgs) {
		usage();
	}

    createstack(&stack,ArraySize);

    for (int i = 0; i < 5; i++) {
        val = (Valor*)malloc(sizeof(Valor));

        val->v = i;
        val->id = 3;
        push(&stack, val);

        //free(val);
    }

    /*val1.id = 2;
    val1.v  = 1;
    push(&stack, &val1);

    val2.id = 3;
    val2.v  = 2;
    push(&stack, &val2);*/

    /*val3.id = 773;
    val3.v  = 24;
    push(&stack, &val3);*/

    for (int i = 0; i < 10; i++) {
        //val = (Valor*)pop(&stack);
        val = (Valor*)pop(&stack);

    
        if(val){
            fprintf(stderr,"My pop: id=%d, v=%d\n", val->id, val->v);
            free(val);
        }
        // aquí llegarían cuando sean NULL
    }

    // si ocurre el caso de más push que pop, hay que tener
    // en cuenta hacer pop hasta que se igualen los valores
    // cuando se destruya la pila

    /*Valor *value = (Valor*)pop(&stack);
    
    if(value){
        fprintf(stderr,"My pop: id=%d, v=%d\n", value->id, value->v);
    }

    Valor *value2 = (Valor*)pop(&stack);
    if(value2){
        fprintf(stderr,"My pop: id=%d, v=%d\n", value2->id, value2->v);
    }*/


    destroystack(&stack);
    //stack = create_stack(100);

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
