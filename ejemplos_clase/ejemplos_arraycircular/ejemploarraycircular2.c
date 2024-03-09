#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <err.h>

struct CircularArray{
    int *array;
    int front;
    int rear;
    int capacity;
};
 
typedef struct CircularArray CircularArray;

CircularArray* createCircularArray(int capacity) {
    CircularArray *circularArray = (CircularArray*)malloc(sizeof(CircularArray));
    circularArray->array = (int*)malloc(capacity * sizeof(int));
    circularArray->front = -1;
    circularArray->rear = -1;
    circularArray->capacity = capacity;
    return circularArray;
}

int isEmpty(CircularArray *circularArray) {
    return (circularArray->front == -1);
}

// coincide posiciones de rear y front 
int isFull(CircularArray *circularArray) {
    return ((circularArray->rear + 1) % circularArray->capacity == circularArray->front);
}

void enqueue(CircularArray *circularArray, int data) {
    if (isFull(circularArray)) {
        printf("Circular array is full\n");
        return;
    }
    if (isEmpty(circularArray))
        circularArray->front = 0;
    circularArray->rear = (circularArray->rear + 1) % circularArray->capacity;
    circularArray->array[circularArray->rear] = data;
}

int dequeue(CircularArray *circularArray) {
    int data;
    if (isEmpty(circularArray)) {
        printf("Circular array is empty\n");
        return -1;
    }
    data = circularArray->array[circularArray->front];
    if (circularArray->front == circularArray->rear)
        circularArray->front = circularArray->rear = -1;
    else
        circularArray->front = (circularArray->front + 1) % circularArray->capacity;
    return data;
}

void display(CircularArray *circularArray) {
    int i;
    if (isEmpty(circularArray)) {
        printf("Circular array is empty\n");
        return;
    }
    printf("Elements in circular array are:\n");
    for (i = circularArray->front; i != circularArray->rear; i = (i + 1) % circularArray->capacity)
        printf("%d ", circularArray->array[i]);
    printf("%d\n", circularArray->array[i]);
}

void freeCircularArray(CircularArray *circularArray) {
    free(circularArray->array);
    free(circularArray);
}


long
getnumber(char *str)
{
	int base;
	char *endptr;
	long val;

	base = 10;

	// Para distinguir entre success/failure después de la llamada
	errno = 0;
	val = strtol(str, &endptr, base);

	// Se comprueban posibles errores 
	if (errno != 0) {
		err(EXIT_FAILURE, "strtol");
	}

	if (endptr == str) {
		errx(EXIT_FAILURE, "No digits were found");
	}
	// Si llegamos hasta aquí strtol() ha sido capaz de parsear un número
	// Ahora es necesario comprobar si la string ha sido un número o no

	if (*endptr != '\0') {
		errx(EXIT_FAILURE, "is not a complete number");
	}
	return val;
}

int
main(int argc, char *argv[]){

    argc--;
    argv++;
    
    CircularArray *circularArray = createCircularArray(getnumber(argv[0]));
    
    enqueue(circularArray, 1);
    enqueue(circularArray, 2);
    enqueue(circularArray, 3);
    enqueue(circularArray, 4);
    display(circularArray);
    
    printf("Dequeued element: %d\n", dequeue(circularArray));
    printf("Dequeued element: %d\n", dequeue(circularArray));
    display(circularArray);
    
    enqueue(circularArray, 5);
    enqueue(circularArray, 6);
    display(circularArray);

    freeCircularArray(circularArray);
    
    return 0;
}