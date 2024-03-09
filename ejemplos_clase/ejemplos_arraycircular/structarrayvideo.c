#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <err.h>

/*enum
{
    N = 5 // size
};*/

/*int queue[N];
int front = -1;
int end = -1;*/

struct Queue{ 

    int *queue;
    int front;
    int end;
    long size;
};
typedef struct Queue Queue; 

void
enqueue(Queue *q, int x)
{
    if(q->front == -1 && q->end == -1){
        q->front = q->end = 0;
        q->queue[q->end] = x;
    }else if (((q->end + 1) % q->size) == q->front){ 
        printf("queue is full\n");
    }else{
        q->end = (q->end + 1) % q->size;
        q->queue[q->end] = x;
    }
}

/*int
dequeue(Queue *q)
{   
    int data;

    if(q->front == -1 && q->end == -1){
        printf("queue is empty\n");
        return -1;
    }

    if (q->front == q->end){
        q->front = q->end = -1;
    }else{
        printf("%d\n", q->queue[q->front]);
        q->front = (q->front + 1) % q->size;
    }
    q->queue[q->front];

   data = q->queue[q->front];
    if (q->front == q->end)
        q->front = q->end = -1;
    else
        q->front = (q->front + 1) % q->size;
    return data;
}*/

void
dequeue(Queue *q)
{   
    //int data;

    if(q->front == -1 && q->end == -1){
        printf("queue is empty\n");
        return;
    }

    /*if (q->front == q->end){
        q->front = q->end = -1;
    }else{
        printf("%d\n", q->queue[q->front]);
        q->front = (q->front + 1) % q->size;
    }
    q->queue[q->front];*/

    //data = q->queue[q->front];
    if (q->front == q->end)
        q->front = q->end = -1;
    else
        q->front = (q->front + 1) % q->size;
    //return data;
}

void
display(Queue *q)
{
    int i = q->front;
    
    if(q->front == -1 && q->end == -1){
        printf("queue is empty\n");
        return;
    }else{
        printf("Queue is: ");
        while( i != q->end ){
            printf("%d ", q->queue[i]);
            i = (i+1) % q->size;
        }
    }
    printf("%d\n", q->queue[q->end]);
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
    // create circular array 
    Queue q;
    q.size = getnumber(argv[0]);
    q.queue = (int *)malloc(sizeof(int)* q.size);
    q.front = -1;
    q.end = -1;

    enqueue(&q,2);
    enqueue(&q,1);
    enqueue(&q,6);
    enqueue(&q,-1);
    enqueue(&q,-2);
    //dequeue(&q);
    dequeue(&q);
    dequeue(&q);


    //enqueue(&q,-3);
    display(&q);

    free(q.queue);
    
    exit(EXIT_SUCCESS);
}