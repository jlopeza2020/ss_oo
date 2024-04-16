// main.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "stack.h"

#define NUM_THREADS 100
#define NUM_VALUES_PER_THREAD 100
#define NUM_VALUES_TO_POP 40

typedef struct {
    int v;
    int id;
} Valor;

Stack *stack;
int incorrect_pops = 0;

void* thread_function(void *arg) {
    int id = *((int*)arg);

    for (int i = 0; i < NUM_VALUES_PER_THREAD; i++) {
        Valor *val = (Valor*)malloc(sizeof(Valor));
        val->v = i;
        val->id = id;
        push(stack, val);
    }

    for (int i = 0; i < NUM_VALUES_TO_POP; i++) {
        Valor *val = (Valor*)pop(stack);
        if (val && val->id != id) {
            incorrect_pops++;
            printf("Incorrect pop: id=%d, v=%d\n", val->id, val->v);
        }
        free(val);
    }

    return NULL;
}

int main() {
    stack = create_stack(100);

    pthread_t threads[NUM_THREADS];
    int thread_args[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_args[i] = i;
        pthread_create(&threads[i], NULL, thread_function, (void*)&thread_args[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Incorrect pops: %d\n", incorrect_pops);
    printf("Remaining items in stack: %d\n", size(stack));

    while (!is_empty(stack)) {
        Valor *val = (Valor*)pop(stack);
        printf("Remaining item: id=%d, v=%d\n", val->id, val->v);
        free(val);
    }

    destroy_stack(stack);
    return 0;
}
