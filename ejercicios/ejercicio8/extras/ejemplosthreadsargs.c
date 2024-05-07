#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 3

// Estructura para los argumentos de los hilos
typedef struct {
    int id;
    volatile int running; // Variable para indicar si el hilo debe seguir ejecutándose
} ThreadArgs;

// Función ejecutada por cada hilo
void *thread_function(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    while (args->running) {
        printf("Thread %d running...\n", args->id);
        sleep(1); // Simulamos algo de trabajo
    }
    printf("Thread %d stopped.\n", args->id);
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    ThreadArgs thread_args[NUM_THREADS];

    // Inicializamos los argumentos y creamos los hilos
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_args[i].id = i;
        thread_args[i].running = 1; // Inicialmente, todos los hilos están corriendo
        pthread_create(&threads[i], NULL, thread_function, (void *)&thread_args[i]);
    }

    // Simulamos un tiempo de ejecución antes de detener un hilo
    sleep(5);

    // Detenemos un hilo específico (en este caso, el hilo con id 1)
    thread_args[1].running = 0;

    // Esperamos a que solo el hilo con id 1 termine
    pthread_join(threads[1], NULL);

    // Imprimimos un mensaje para indicar que el hilo con id 1 ha terminado
    printf("Thread %d joined and stopped.\n", thread_args[1].id);

    // Ahora esperamos a que los otros hilos terminen
    for (int i = 0; i < NUM_THREADS; i++) {
        // No hacemos join para el hilo con id 1
        if (i != 1) {
            pthread_join(threads[i], NULL);
        }
    }

    printf("All threads stopped.\n");

    return 0;
}
