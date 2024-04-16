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

int 
isemptystack(Stack *st){

    int value;

    pthread_mutex_lock(&st->mutex);
    value = (st->postop == -1);
    pthread_mutex_unlock(&st->mutex);

    return value;
}

// preguntar si está bien o meter la declaración dentro de la región crítica
// no poner locks y unlock porque genera interbloqueos
static int
isempty(Stack *st){
    //int value;

    //pthread_mutex_lock(&st->mutex);
    //value = (st->postop == -1);
    //pthread_mutex_unlock(&st->mutex);

    return (st->postop == -1);
}

static int 
isfull(Stack *st){

    //int value;
    //pthread_mutex_lock(&st->mutex);
    //value = (st->postop == (st->totalsize -1));
    //pthread_mutex_unlock(&st->mutex);

    return (st->postop == (st->totalsize-1));
}

void
push(Stack *st, void *item){
    pthread_mutex_lock(&st->mutex);
    if(isfull(st)){
        st->totalsize *= 2;
        // preguntar si tiene valor de retorno
        st->array = (void**)realloc(st->array, sizeof(void*)*st->totalsize);
        /*if (st->array == NULL) {
            errx(EXIT_FAILURE, "Error: dynamic memory cannot be allocated");
        }*/
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
