#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <errno.h>


// estructura de datos para el 
// buffer circular
struct Circulararray{ 

    int *array;
    int front;
    int end;
    long size;
};
typedef struct Circulararray Circulararray; 

void
usage()
{
	fprintf(stderr,
		"usage: ./ngrams routefile ngramsize\n");
	exit(EXIT_FAILURE);
}

long
getnumber(char *str)
{
	int base;
	char *endptr;
	long val;

	base = 10;

	errno = 0;
	val = strtol(str, &endptr, base);

	// Se comprueban posibles errores 
	if (errno != 0) {
		err(EXIT_FAILURE, "strtol");
	}

	if (endptr == str) {
		errx(EXIT_FAILURE, "No digits were found");
	}

	// Ahora es necesario comprobar si la string ha sido un número o no
	if (*endptr != '\0') {
		errx(EXIT_FAILURE, "is not a complete number");
	}
	return val;
}

int 
isfull(Circulararray *q){
	return (((q->end + 1) % q->size) == q->front);
}

int
isempty(Circulararray *q){
	return (q->front == -1 && q->end == -1);
}

void
enqueue(Circulararray *q, int x)
{
    if(q->front == -1 && q->end == -1){
        q->front = q->end = 0;
        q->array[q->end] = x;
    }
	//else if (((q->end + 1) % q->size) == q->front){ 
	else if (isfull(q)){ 

        printf("circular array is full\n");
    }else{
        q->end = (q->end + 1) % q->size;
        q->array[q->end] = x;
    }
}

void
dequeue(Circulararray *q)
{   
    //if(q->front == -1 && q->end == -1){
	if(isempty(q)){
        printf("circular array is empty\n");
        return;
    }

    if (q->front == q->end)
        q->front = q->end = -1;
    else
        q->front = (q->front + 1) % q->size;
}

void
display(Circulararray *q)
{
    int i = q->front;
    
    //if(q->front == -1 && q->end == -1){
	if(isempty(q)){
        printf("circular array is empty\n");
        return;
    }else{
        printf("Circular Array is: ");
        while( i != q->end ){
            printf("%d ", q->array[i]);
            i = (i+1) % q->size;
        }
    }
    printf("%d\n", q->array[q->end]);
}

int
main(int argc, char *argv[]){

    argc--;
	argv++;

    long ngramsize;

    if(argc != 2){
        usage();
    }
    ngramsize = getnumber(argv[1]);

    if (ngramsize <= 0) {
		errx(EXIT_FAILURE, "second parameter should be bigger than 0");
	}

	// declaración del array circular 
	Circulararray ca;
    ca.size = ngramsize;
    ca.array = (int *)malloc(sizeof(int)* ca.size); // cambiar a char 
    ca.front = -1;
    ca.end = -1;

    enqueue(&ca,2);
    enqueue(&ca,1);
    enqueue(&ca,6);
    //enqueue(&ca,-1);
    //enqueue(&ca,-2);
    //dequeue(&q);
    dequeue(&ca);
    dequeue(&ca);


    //enqueue(&q,-3);
    display(&ca);

    free(ca.array);



    // hay que abrir el fichero en modo lectura 
    // escribir por la salida estándar [valores]
    // fopen (path, "r")
    // fclose(path)

    // fread no actualiza errno 
    // mirar feof y ferror para ver los valores que da
    // fwrite
    // ¿revisar fflush y fgets?

    exit(EXIT_SUCCESS);
}