// stack.h
#ifndef STACK_H
#define STACK_H

#include <pthread.h>

typedef struct {
    void **array;
    int capacity;
    int size;
    pthread_mutex_t mutex;
} Stack;

Stack* create_stack(int capacity);
void destroy_stack(Stack *stack);
int is_empty(Stack *stack);
void* pop(Stack *stack);
void push(Stack *stack, void *item);
int size(Stack *stack);

#endif // STACK_H
