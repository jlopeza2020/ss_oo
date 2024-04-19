
struct Stack {
	void **array;
	long long totalsize;	// tamaño total del array
	long long postop;	// posición de la cima de la pila
	pthread_mutex_t mutex;
};
typedef struct Stack Stack;

Stack *createstack(long long totalsize);
int isempty(Stack *st);
void push(Stack *st, void *item);
void *pop(Stack *st);
long long size(Stack *st);
// aquí no debe de haber ningún hilo usando la pila 
void freestack(Stack *stack);
