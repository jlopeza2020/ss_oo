#include <stdio.h>
#include <stdlib.h>

struct Node{
    int data;
    struct Node *next; 
};
typedef struct Node Node;

struct List{
    // root = init
    Node *init;
    int total;
    Node *last; 
};
typedef struct List List;

// OPERACIONES PARA EL NODOD
Node 
*createnode(int data){

    Node *n = (Node *)malloc(sizeof(Node));
    n->data = data;
    n->next = NULL;

    return n;
}

void
printnode(Node *n){
    printf("%d\n", n->data);
}

// *******************************************************// 
// OPERACIONES PARA LA LISTA
List 
*createlist(){

    List *l = (List *)malloc(sizeof(List));
    l->init = NULL;
    l->total = 0;
    l->last = NULL;

    return l;
}

// es vacío si el nodo inicial no apunta a nada
int 
isempty(List *l){
    if(l->init == NULL){
        return 1;
    }
    return 0;
}

// insertar al final de la lista
void 
insertatend(List *l, Node *n){
    
    // tanto principio como fin apuntan al mismo nodo
    if(isempty(l)){
        l->init = n;
        l->last = n;
    }else{
        // la siguiente posición del último nodo pasa de NULL a n
        l->last->next = n;
        // el nodo final de la lista apunta al nuevo nodo insertado
        l->last = n;
    }
    l->total++;
}

void 
insertatbeggining(List *l, Node *n){
    if(isempty(l)){ 
        l->init = n;
        l->last = n;
    }else{
        // el nodo apunta donde se encuentra la primera
        // posición de la lista 
        n->next= l->init;
        // la primera posición ahora es n
        l->init = n;
    }
    l->total++;
}

void
printlist(List *l){

    Node *aux = l->init;

    while (aux != NULL){
        printnode(aux);
        aux=aux->next;
    }
}

void
elimfirst(List *l){

    Node *aux;

    if(!isempty(l)){

        aux = l->init;
        if(l->total == 1){
            // usamos aux para eliminar la memoria de ese nodo
            l->init = NULL;
            l->last = NULL;
        }else{
            // nodo del principio de la lista 
            // apunta al nodo siguiente 
            l->init = l->init->next; 
        }
        // eliminamos la memoria del nodo a eliminar
        free(aux);
        l->total--;
    }
}


void 
elimlast(List *l){

    Node *aux, *prelast;

    if(!isempty(l)){

        aux = l->last;
        if(l->total == 1){
            // usamos aux para eliminar la memoria de ese nodo
            l->init = NULL;
            l->last = NULL;
        }else{
            // recorrer la lista y posicionarnos en la posición n-1
            prelast = l->init;
            while(prelast->next != l->last){
                prelast = prelast->next;
            }
            // se deja el nodo final de la lista el penúltimo
            l->last = prelast; 
            // último en su atributo siguiente es NULO
            l->last->next = NULL;
        }
        free(aux);
        l->total--;
    }

}
void 
elimbyindex(List *l, int index){
    Node *pre, *aux;
    int i;

    if(index >= 0 && index < l->total){
        if(index == 0){
            elimfirst(l);
        }else if(index == (l->total -1)){
            elimlast(l);
        }else{
            // se define un nodo aux que apunta 
            // al principio de la lista
            aux = l->init;
            for(i = 0; i < index; i++){
                // se usa otro nodo pre que se encunetra una posición
                // previa a la de aux
                pre=aux;
                aux=aux->next;
            }
            // cuando llegamos al índice buscado: 
            // el nodo prev pasa apuntar como nodo siguiente al que está 
            // apuntando aux
            pre->next=aux->next;
            // como ya la lista está completamente enlazada,
            // el valor de aux no nos hace falta y podemo liberarlo
            free(aux);
            l->total--;

        }

    }
}

void
elimbyvalue(List *l, int data){
    int i;
    Node *aux;
    i = 0;
    aux = l->init;

    while(aux != NULL && aux->data != data){
        aux = aux->next;
        i++;
    }
    // Hemos encontrado el nodo y por lo tanto sabemos su índice
    if(aux != NULL){
        elimbyindex(l,i);
    }
}

Node *
searchbyindex(List *l, int index){

    Node *aux = l->init;
    
    int i;
    for(i = 0; i<index; i++){
        aux=aux->next;
    }
    return aux; 
}

Node *
searchbyvalue(List *l, int value){

    Node *aux = l->init;
    
    while(aux != NULL && aux->data != value){
        aux=aux->next;
    }
    return aux; 
}

void
emptylist(List *l){
    while(!isempty(l)){
        elimfirst(l);
    }
    free(l);
}

int 
main(int argc, char *argv[]){

    Node *n;
    List *l;
    l = createlist();
    insertatend(l,createnode(47));
    insertatend(l,createnode(10));
    insertatend(l,createnode(4));
    insertatend(l,createnode(5));
    //printlist(l);
    elimbyvalue(l,5);
    printlist(l);

    n=searchbyvalue(l, 4);
    if(n!=NULL){
        printnode(n);
    }
    emptylist(l);

    exit(EXIT_SUCCESS);
}