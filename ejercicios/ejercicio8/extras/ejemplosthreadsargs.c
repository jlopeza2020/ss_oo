#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

// Define la estructura ThreadArgs y su alias ThreadArgs
struct Node{
    // MIRAR SI INCLUIR EN EL NODO EL FILE * de cada nodo
    long long score;
    char *name;
    int id; // es el id del thread
    //int fd;
    FILE *fd;
    struct Node *next; 
};
typedef struct Node Node;

struct List{

    Node *init;
    int total;
    Node *last; 
    pthread_mutex_t listmutex;

};
typedef struct List List;

struct ThreadArgs {
    List *list;
    char *name;
    int id;
    FILE *fd;
    pthread_t *threads;
};
typedef struct ThreadArgs ThreadArgs;

int main() {
    // Crea una instancia de ThreadArgs
    ThreadArgs *args = malloc(sizeof(ThreadArgs));

    // Asigna valores a los miembros de la estructura
    args->list = NULL; // O asigna un puntero válido a una estructura List
    args->name = strdup("Nombre del hilo"); // O asigna un nombre válido
    args->id = 123; // O asigna un ID válido
    args->fd = fopen("/tmp/archivo.txt", "r"); // O abre un archivo con fopen() y asigna el puntero FILE

    // Asigna memoria para el array de pthread_t
    int num_threads = 5; // Por ejemplo, asumiendo que tienes 5 threads
    args->threads = malloc(num_threads * sizeof(pthread_t));

    // Luego de utilizar la estructura, no te olvides de liberar la memoria asignada
    free(args->name);
    if (args->fd != NULL) {
        fclose(args->fd);
    }
    free(args->threads[0]);
    free(args);

    return 0;
}
