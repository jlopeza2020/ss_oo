#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

typedef struct CircularBuffer {
    char buffer[BUFFER_SIZE];
    int front;
    int rear;
    int size;
} CircularBuffer;

void initializeBuffer(CircularBuffer *cb) {
    cb->front = 0;
    cb->rear = -1;
    cb->size = 0;
}

void enqueue(CircularBuffer *cb, char c) {
    if (cb->size == BUFFER_SIZE) {
        printf("Buffer overflow\n");
        return;
    }
    
    cb->rear = (cb->rear + 1) % BUFFER_SIZE;
    cb->buffer[cb->rear] = c;
    cb->size++;
}

char dequeue(CircularBuffer *cb) {
    if (cb->size == 0) {
        printf("Buffer underflow\n");
        return '\0';
    }

    char item = cb->buffer[cb->front];
    cb->front = (cb->front + 1) % BUFFER_SIZE;
    cb->size--;
    return item;
}

void printNGrams(const char *filename, int n) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file '%s'\n", filename);
        exit(EXIT_FAILURE);
    }

    CircularBuffer cb;
    initializeBuffer(&cb);

    char c;
    while (fread(&c, sizeof(char), 1, file) != 0) {
        enqueue(&cb, c);
        if (cb.size == n) {
            printf("[");
            for (int i = 0; i < n; i++) {
                printf("%c", cb.buffer[(cb.front + i) % BUFFER_SIZE]);
            }
            printf("]\n");
            dequeue(&cb);
        }
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filename> <n>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *filename = argv[1];
    int n = atoi(argv[2]);

    if (n <= 0) {
        fprintf(stderr, "Error: n must be a positive integer\n");
        return EXIT_FAILURE;
    }

    printNGrams(filename, n);

    return EXIT_SUCCESS;
}
