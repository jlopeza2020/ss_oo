#include <pthread.h>
#include <stdlib.h>
#include "stack.h"

// createstack
/*Stack* create_stack(int capacity) {
    Stack *stack = (Stack*)malloc(sizeof(Stack));
    stack->array = (void**)malloc(capacity * sizeof(void*));
    stack->capacity = capacity;
    stack->size = 0;
    pthread_mutex_init(&stack->mutex, NULL);
    return stack;
}*/
void
createstack(Stack *st, long long arraysize){

    // hace falta hacer malloc de la estructura ???
    st->array = (void **)malloc(sizeof(void*)*arraysize);
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


// no puede ser pública (CAMBIAR!!!!!!!!!!!)
/*void destroy_stack(Stack *stack) {
    free(stack->array);
    pthread_mutex_destroy(&stack->mutex);
    free(stack);
}*/

// isempty
/*int is_empty(Stack *stack) {
    pthread_mutex_lock(&stack->mutex);
    int result = stack->size == 0;
    pthread_mutex_unlock(&stack->mutex);
    return result;
}*/

/*void* pop(Stack *stack) {
    pthread_mutex_lock(&stack->mutex);
    if (stack->size == 0) {
        pthread_mutex_unlock(&stack->mutex);
        return NULL;
    }
    void *item = stack->array[--stack->size];
    pthread_mutex_unlock(&stack->mutex);
    return item;
}*/

// usar realloc
/*void push(Stack *stack, void *item) {
    pthread_mutex_lock(&stack->mutex);
    if (stack->size == stack->capacity) {
        stack->capacity *= 2;
        stack->array = (void**)realloc(stack->array, stack->capacity * sizeof(void*));
    }
    stack->array[stack->size++] = item;
    pthread_mutex_unlock(&stack->mutex);
}*/

// numelements
/*int size(Stack *stack) {
    pthread_mutex_lock(&stack->mutex);
    int size = stack->size;
    pthread_mutex_unlock(&stack->mutex);
    return size;
}*/
