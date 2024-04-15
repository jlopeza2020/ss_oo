enum{
    ZeroArgs,
    NThreads = 100,
    NPush = 100,
    NPop  = 40,
    ArraySize = 1,
};

struct Stack {
    void **array;
    long long totalsize; // tamaño total del array
    long long postop;  // posición de la cima de la pila
    //pthread_mutex_t mutex;
};
typedef struct Stack Stack;

// revisar si declararla aquí o en el main 
struct Valor {
	int v;
    int id;
};
typedef struct Valor Valor;


void createstack(Stack *st, long long totalsize);
int isempty(Stack *st);
void push(Stack *st, void *item);
void* pop(Stack *st);
int size(Stack *st);

