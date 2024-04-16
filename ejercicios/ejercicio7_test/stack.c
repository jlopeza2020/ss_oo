// stack.c
#include "stack.h"
#include <stdlib.h>

Stack* create_stack(int capacity) {

    Stack *stack = (Stack*)malloc(sizeof(Stack));
    stack->array = (void**)malloc(capacity * sizeof(void*));
    stack->capacity = capacity;
    stack->size = 0;
    pthread_mutex_init(&stack->mutex, NULL);
    return stack;
}

void destroy_stack(Stack *stack) {
    free(stack->array);
    pthread_mutex_destroy(&stack->mutex);
    free(stack);
}

int is_empty(Stack *stack) {
    pthread_mutex_lock(&stack->mutex);
    int result = stack->size == 0;
    pthread_mutex_unlock(&stack->mutex);
    return result;
}

void* pop(Stack *stack) {
    pthread_mutex_lock(&stack->mutex);
    if (stack->size == 0) {
        pthread_mutex_unlock(&stack->mutex);
        return NULL;
    }
    void *item = stack->array[--stack->size];
    pthread_mutex_unlock(&stack->mutex);
    return item;
}

void push(Stack *stack, void *item) {
    pthread_mutex_lock(&stack->mutex);
    if (stack->size == stack->capacity) {
        stack->capacity *= 2;
        stack->array = (void**)realloc(stack->array, stack->capacity * sizeof(void*));
    }
    stack->array[stack->size++] = item;
    pthread_mutex_unlock(&stack->mutex);
}

int size(Stack *stack) {
    pthread_mutex_lock(&stack->mutex);
    int size = stack->size;
    pthread_mutex_unlock(&stack->mutex);
    return size;
}
