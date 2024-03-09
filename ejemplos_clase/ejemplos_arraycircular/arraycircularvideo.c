#include <stdlib.h>
#include <stdio.h>

enum
{
    N = 5 // size
};

int queue[N];
int front = -1;
int end = -1;

void
enqueue(int x)
{
    if(front == -1 && end == -1){
        front = end = 0;
        queue[end] = x;
    }else if (((end + 1) % N) == front){
        printf("queue is full\n");
    }else{
        end = (end + 1) % N;
        queue[end] = x;
    }
}

void
dequeue()
{   
    if(((front + 1) % N) == end){
        printf("queue is full\n");
    }else if (front == end){
        front = end = -1;
    }else{
        printf("%d\n", queue[front]);
        front = (front + 1) % N;
    }
}

void
display()
{
    int i = front;
    
    if(front == -1 && end == -1){
        printf("queue is empty\n");
    }else{
        printf("Queue is: ");
        while( i != end ){
            printf("%d ", queue[i]);
            i = (i+1) % N;
        }
    }
    printf("%d\n", queue[end]);
}

int
main(){

    enqueue(2);
    enqueue(1);
    enqueue(0);
    enqueue(-1);
    enqueue(-2);
    dequeue();
    enqueue(-3);
    display();
    
    exit(EXIT_SUCCESS);
}