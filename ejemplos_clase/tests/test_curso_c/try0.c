#include <stdio.h>
#include <string.h>


// Función de ordenación de burbuja para ordenar los argumentos
void bubble_sort(char *args[], int n) {
    int i, j;
    char *temp;
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            if (strcmp(args[j], args[j + 1]) > 0) {
                
                //printf("hay que intercambiar %s por %s\n", args[j], args[j + 1]);

                temp = args[j];
                args[j] = args[j + 1];
                args[j + 1] = temp;
            }
        }
    }
}

int main(int argc, char *argv[]) {

    // Ordenar los argumentos
    bubble_sort(argv + 1, argc - 1);

    // Imprimir los argumentos ordenados
    for (int i = 1; i < argc; i++) {
        printf("%s\n", argv[i]);
    }

    return 0;
}
