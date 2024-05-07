#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

// tiene que tener nombre de usuario y puntuación 
struct Node{
    long long score;
    char *name;
    int id;
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
*createnode(char *name, long long score, int id){

    Node *n = (Node *)malloc(sizeof(Node));
    if (n == NULL) {
		errx(EXIT_FAILURE, "Error: dynamic memory cannot be allocated");
	}

    n->name = (char *)malloc(sizeof(char)*1024);
    if (name == NULL) {
		errx(EXIT_FAILURE, "Error: dynamic memory cannot be allocated");
	}

    strcpy(n->name,name);

    n->score = score;
    n->id = id;
    n->next = NULL;

    return n;
}

void
printnode(Node *n){
    printf("%s:%lld\n", n->name,n->score);
}

// *******************************************************// 
// OPERACIONES PARA LA LISTA
List 
*createlist(){

    List *l = (List *)malloc(sizeof(List));
    if (l == NULL) {
		errx(EXIT_FAILURE, "Error: dynamic memory cannot be allocated");
	}
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
        free(aux->name);
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
        free(aux->name);
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
            free(aux->name);
            free(aux);
            l->total--;

        }

    }
}

void
elimbyvalue(List *l, long long score){
    int i;
    Node *aux;
    i = 0;
    aux = l->init;

    while(aux != NULL && aux->score != score){
        aux = aux->next;
        i++;
    }
    // Hemos encontrado el nodo y por lo tanto sabemos su índice
    if(aux != NULL){
        elimbyindex(l,i);
    }
}

void
elimbyname(List *l, char *name){
    int i;
    Node *aux;
    i = 0;
    aux = l->init;

    while(aux != NULL && strcmp(aux->name, name)!= 0){
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
    
    while(aux != NULL && aux->score != value){
        aux=aux->next;
    }
    return aux; 
}

Node *
searchbyname(List *l, char *name){

    Node *aux = l->init;

    while(aux != NULL && strcmp(aux->name, name)!= 0){
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


void
changevalue(List *l, char *name, long long value){

    Node *n;
    n = searchbyname(l,name);
    if(n != NULL){
        if(n->score < value){
            n->score = value;
        }
    }
}

void
changeallvalues(List *l, long long value){


    Node *aux = l->init;


    while (aux != NULL){
        aux->score  = 0;
        aux=aux->next;
    }
}


// TRATAR LO DE LOS MAX JUGADORES
int 
main(int argc, char *argv[]){

    Node *n;
    List *l;
    int i;
    
    i = 0;
    l = createlist();
    insertatend(l,createnode("Manuel", 270000077, i));
    i++;
    insertatend(l,createnode("Pedro", 25, i));
    i++;

    insertatend(l,createnode("Lucía",100000, i));
    i++;
    insertatend(l,createnode("María", 564, i));
    i++;

    //printlist(l);
    //elimbyvalue(l,27);
    elimbyname(l,"María");

    printlist(l);

    //n=searchbyvalue(l, 4);
    fprintf(stderr,"soy\n");
    n=searchbyname(l, "Manuel");
    if(n!=NULL){
        printnode(n);
    }

    changevalue(l,"Lucía", 70567);

    printlist(l);

    changeallvalues(l, 0);

    emptylist(l);
    exit(EXIT_SUCCESS);
}