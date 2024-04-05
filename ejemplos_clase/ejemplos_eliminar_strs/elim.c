#include <stdio.h>
#include <string.h>

// Función para eliminar una cadena de un array de cadenas
void eliminarCadena(char arr[][100], int *tamano, const char *cadena) {
    int i, j;

    // Buscar la cadena en el array
    for (i = 0; i < *tamano; i++) {
        if (strcmp(arr[i], cadena) == 0) {
            // Si se encuentra la cadena, eliminarla
            for (j = i; j < *tamano - 1; j++) {
                strcpy(arr[j], arr[j + 1]);
            }
            (*tamano)--;
            i--; // Disminuir el índice para evitar omitir un elemento
        }
    }
}

int main() {
    // Definir un array de cadenas
    char array_strings[5][100] = {"cadena1", "cadena2", "cadena3", "cadena4", "cadena5"};
    int tamano = 5;

    // Eliminar la cadena "cadena3"
    eliminarCadena(array_strings, &tamano, "cadena3");

    // Mostrar el array después de la eliminación
    for (int i = 0; i < tamano; i++) {
        printf("%s\n", array_strings[i]);
    }

    return 0;
}
