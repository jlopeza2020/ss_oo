#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // Array dinámico original
    char **array = (char **)malloc(3 * sizeof(char *));
    array[0] = strdup("ls");
    array[1] = strdup("*");
    array[2] = strdup("/tmp");
    int array_length = 3;

    // Nuevo contenido para insertar
    char *nuevo_contenido[] = {"hola", "adios"};
    int nuevo_contenido_length = sizeof(nuevo_contenido) / sizeof(nuevo_contenido[0]);

    // Buscar el índice del asterisco en el array
    int asterisco_index = -1;
    for (int i = 0; i < array_length; i++) {
        if (strcmp(array[i], "*") == 0) {
            asterisco_index = i;
            break;
        }
    }

    // Si se encuentra el asterisco, reemplazarlo con el nuevo contenido
    if (asterisco_index != -1) {
        // Calcular el nuevo tamaño del array
        int nuevo_tamano = array_length + nuevo_contenido_length -1; // -1 porque vamos a quitar el asterisco

        // Reasignar memoria para el array con el nuevo tamaño
        array = (char **)realloc(array, nuevo_tamano * sizeof(char *));

        // Desplazar elementos para hacer espacio para el nuevo contenido
        for (int i = array_length - 1; i >= asterisco_index + 1; i--) {
            array[i + nuevo_contenido_length - 1] = array[i];
        }

        // Insertar el nuevo contenido en el array
        for (int i = 0; i < nuevo_contenido_length; i++) {
            array[asterisco_index + i] = strdup(nuevo_contenido[i]);
        }

        // Actualizar la longitud del array
        array_length = nuevo_tamano;
    }

    // Imprimir el array resultante
    printf("Array final:\n");
    for (int i = 0; i < array_length; i++) {
        printf("%s\n", array[i]);
        free(array[i]); // Liberar la memoria asignada por strdup
    }
    free(array); // Liberar la memoria asignada para el array

    return 0;
}
