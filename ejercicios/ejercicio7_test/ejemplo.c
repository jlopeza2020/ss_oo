#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 10

typedef struct Stack {
    void **array;
    long long totalsize; // tamaño total del array
    long long postop;    // posición de la cima de la pila
    //pthread_mutex_t mutex;
} Stack;

typedef struct {
    int id;
    Stack *stack;
} ThreadData;

void *thread_function(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    // Acceder a la pila desde los datos del hilo
    /////Stack *stack = data->stack;

    // Hacer algo con la pila en el hilo
    // Ejemplo:
    // Operaciones en la pila...

    free(data); // Liberar la memoria asignada para los datos

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    ThreadData *data[NUM_THREADS];
    Stack *stack;

    // Crear y asignar memoria para la estructura de pila
    stack = (Stack *)malloc(sizeof(Stack));
    if (stack == NULL) {
        perror("Error allocating memory for stack");
        exit(EXIT_FAILURE);
    }

    // Inicializar la pila y otros datos si es necesario

    // Crear los hilos y asignar datos
    for (int i = 0; i < NUM_THREADS; i++) {
        data[i] = (ThreadData *)malloc(sizeof(ThreadData));
        if (data[i] == NULL) {
            perror("Error allocating memory for thread data");
            // Liberar memoria asignada previamente
            for (int j = 0; j < i; j++) {
                free(data[j]);
            }
            free(stack);
            exit(EXIT_FAILURE);
        }
        data[i]->id = i;
        data[i]->stack = stack;

        // Crear el hilo y pasar los datos como argumento
        if (pthread_create(&threads[i], NULL, thread_function, (void *)data[i]) != 0) {
            perror("Error creating thread");
            // Cancelar hilos previamente creados
            for (int j = 0; j < i; j++) {
                pthread_cancel(threads[j]);
            }
            // Liberar memoria asignada previamente
            for (int j = 0; j < i; j++) {
                free(data[j]);
            }
            free(stack);
            exit(EXIT_FAILURE);
        }
    }

    // Esperar a que los hilos terminen
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Error joining thread");
            // Cancelar hilos previamente creados
            for (int j = 0; j < NUM_THREADS; j++) {
                pthread_cancel(threads[j]);
            }
            // Liberar memoria asignada previamente
            for (int j = 0; j < NUM_THREADS; j++) {
                free(data[j]);
            }
            free(stack);
            exit(EXIT_FAILURE);
        }
    }

    // Liberar la memoria asignada para la estructura de pila y datos
    free(stack);
    /*for (int i = 0; i < NUM_THREADS; i++) {
        free(data[i]);
    }*/

    return 0;
}
