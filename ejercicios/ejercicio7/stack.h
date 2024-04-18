enum{
    ZeroArgs,
    NThreads = 10,
    NPush = 50,
    NPop  = 20,
    ArraySize = 1,
};

struct Stack {
    void **array;
    long long totalsize; // tamaño total del array
    long long postop;  // posición de la cima de la pila
    pthread_mutex_t mutex;
};
typedef struct Stack Stack;

struct Valor {
	int v;
    int id;
};
typedef struct Valor Valor;

struct ThreadArgs{
    Stack *stack;
    int id;
};
typedef struct ThreadArgs ThreadArgs;

Stack *createstack(long long totalsize);
int isempty(Stack *st);
void push(Stack *st, void *item);
void* pop(Stack *st);
long long  size(Stack *st);
void freestack(Stack *stack);

