#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

// Estructura de datos para un buffer circular
typedef struct {
    char data[BUFFER_SIZE];
    int start;
    int end;
    int count;
} CircularBuffer;

// Función para inicializar el buffer
void initBuffer(CircularBuffer *buffer) {
    buffer->start = 0;
    buffer->end = 0;
    buffer->count = 0;
}

// Función para añadir un caracter al buffer
void addToBuffer(CircularBuffer *buffer, char c) {
    buffer->data[buffer->end] = c;
    buffer->end = (buffer->end + 1) % BUFFER_SIZE;
    if (buffer->count < BUFFER_SIZE)
        buffer->count++;
    else
        buffer->start = (buffer->start + 1) % BUFFER_SIZE;
}

// Función para imprimir todos los ngramas de tamaño n en el buffer
void printNgrams(CircularBuffer *buffer, int n) {
    int i, j;
    for (i = buffer->start; i < buffer->start + buffer->count; i++) {
        for (j = 0; j < n; j++) {
            putchar(buffer->data[(i + j) % BUFFER_SIZE]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <ruta_del_fichero> <tamaño_de_los_ngramas>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Error al abrir el fichero");
        return 1;
    }

    int n = atoi(argv[2]);
    if (n <= 0) {
        printf("Tamaño de ngramas no válido.\n");
        return 1;
    }

    CircularBuffer buffer;
    initBuffer(&buffer);

    char c;
    while ((c = fgetc(file)) != EOF) {
        addToBuffer(&buffer, c);
        if (buffer.count >= n)
            printNgrams(&buffer, n);
    }

    fclose(file);
    return 0;
}
