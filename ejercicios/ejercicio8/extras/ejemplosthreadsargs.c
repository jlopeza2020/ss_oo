#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Estructura para los argumentos del hilo
struct ThreadArgs {
    int *running_flag; // Señal de finalización
};

// Función que ejecutará el hilo
void *thread_func(void *arg) {
    struct ThreadArgs *args = (struct ThreadArgs *)arg;
    int *running_flag = args->running_flag; // Obtener el puntero a la señal de finalización

    // Bucle principal del hilo
    while (*running_flag) {
        printf("Thread running...\n");
        sleep(1); // Simular una operación de larga duración
    }

    printf("Thread exiting...\n");

    pthread_exit(NULL);
}

int main() {
    pthread_t thread;
    struct ThreadArgs args;

    // Inicializar la señal de finalización
    int running = 1;
    args.running_flag = &running;

    // Crear el hilo
    if (pthread_create(&thread, NULL, thread_func, (void *)&args) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    // Esperar un tiempo antes de enviar la señal de finalización
    //sleep(3);

    // Enviar la señal de finalización al hilo
    printf("Sending termination signal...\n");
    running = 0;
    if(!running){
        // Esperar a que el hilo termine
        if (pthread_join(thread, NULL) != 0) {
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }
    }

    printf("Main thread exiting...\n");

    return 0;
}
