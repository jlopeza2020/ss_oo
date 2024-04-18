#include <pthread.h>
#include <stdlib.h>
#include <err.h>
#include "stack.h"

Stack *
createstack(long long arraysize){

    // hace falta hacer malloc de la estructura ???
    Stack *st = (Stack *)malloc(sizeof(Stack));
    if (st == NULL) {
		errx(EXIT_FAILURE, "Error: dynamic memory cannot be allocated");
	}
    st->array = (void **)malloc(sizeof(void*)*arraysize);
    if (st->array == NULL) {
		errx(EXIT_FAILURE, "Error: dynamic memory cannot be allocated");
	}
    st->postop = -1;
    st->totalsize = arraysize;
    // incluir el mutex
    pthread_mutex_init(&st->mutex, NULL);

    return st;
}

// coge el cierre llamarla isempty
int 
isemptystack(Stack *st){

    int value;

    pthread_mutex_lock(&st->mutex);
    value = (st->postop == -1);
    pthread_mutex_unlock(&st->mutex);

    return value;
}

// no poner locks y unlock porque genera interbloqueos
// llamarla _isempty no coge el cierre 
static int
isempty(Stack *st){
    return (st->postop == -1);
}

static int 
isfull(Stack *st){
    return (st->postop == (st->totalsize-1));
}

void
push(Stack *st, void *item){
    pthread_mutex_lock(&st->mutex);
    if(isfull(st)){
        st->totalsize *= 2;
        // preguntar si tiene valor de retorno
        st->array = (void**)realloc(st->array, sizeof(void*)*st->totalsize);
    }
    st->postop++;
    st->array[st->postop] = item;
    pthread_mutex_unlock(&st->mutex);

}
void *
pop(Stack *st){

    void *popelement;

    pthread_mutex_lock(&st->mutex);
    if(isempty(st)){
        pthread_mutex_unlock(&st->mutex);
        return NULL;
    }
    popelement = st->array[st->postop];
    st->postop--;
    pthread_mutex_unlock(&st->mutex);

    return popelement; 
}

long long 
size(Stack *st){
    long long size;

    pthread_mutex_lock(&st->mutex);
    size = st->postop + 1;
    pthread_mutex_unlock(&st->mutex);
    
    return size;
}


// se puede hacer pública
void 
freestack(Stack *stack){
    free(stack->array);
    pthread_mutex_destroy(&stack->mutex);
    free(stack);
}