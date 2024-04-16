#include <pthread.h>
#include <stdlib.h>
#include <err.h>
#include "stack.h"

void
createstack(Stack *st, long long arraysize){

    // hace falta hacer malloc de la estructura ???
    st->array = (void **)malloc(sizeof(void*)*arraysize);
    if (st->array == NULL) {
		errx(EXIT_FAILURE, "Error: dynamic memory cannot be allocated");
	}
    st->postop = -1;
    st->totalsize = arraysize;
    // incluir el mutex
}

int
isempty(Stack *st){
    return (st->postop == -1);
}

static int 
isfull(Stack *st){
    return (st ->postop == (st->totalsize -1));
}

void
push(Stack *st, void *item){
    if(isfull(st)){
        st->totalsize *= 2;
        // preguntar si tiene valor de retorno
        st->array = (void**)realloc(st->array, sizeof(void*)*st->totalsize);

    }
    st->postop++;
    st->array[st->postop] = item;
}
void *
pop(Stack *st){

    void *popelement;

    if(isempty(st)){
        return NULL;
    }
    popelement = st->array[st->postop];
    st->postop--;

    return popelement; 
}

int
size(Stack *st){

    return st->postop + 1;
}
