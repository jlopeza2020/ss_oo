#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>

// quitar el usuario de la tabla es complejo 
// hacer un cleanup 
// el principal sale cuando acaben todos 
// no condicienes de carrera, los fifos funcionen correctamente 
// el cleanp ordenado sin condiciones de carrera y demás 

// ******************* DEFINICIONES PARA LA TABLA ****************************
struct Node{
    // MIRAR SI INCLUIR EN EL NODO EL FILE * de cada nodo
    long long score;
    char *name;
    int id; // es el id del thread
    struct Node *next; 
};
typedef struct Node Node;

struct List{

    Node *init;
    int total;
    Node *last; 
    // Dentro de la estructura lista incluir un mutex !!!!
    pthread_mutex_t listmutex;

};
typedef struct List List;

// ***************************************************************************

// ***************** DEFINICIÓN PARA COMANDOS ******************************
enum {
    MaxPlayers = 50, // TENGO QUE INCLUIRLO
    LineSz = 2*1024,

};

enum {
    Newplayer, 
    Delplayer,
    Highscore,
    Reset,
    Ncommnads,
}; 

char  *commands[Ncommnads] = {"newplayer", "delplayer", "highscore", "reset"};

struct Command{
    char *command;
    char *arg; 
};
typedef struct Command Command;

struct ThreadArgs {
	List *list;
    char *name;
    int id;
};
typedef struct ThreadArgs ThreadArgs;
// ***************************************************************************

// ******************** OPERACIONES PARA EL NODO *****************************
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

// ***************************************************************************
// ******************** OPERACIONES PARA LA LISTA ****************************
List 
*createlist(){

    List *l = (List *)malloc(sizeof(List));
    if (l == NULL) {
		errx(EXIT_FAILURE, "Error: dynamic memory cannot be allocated");
	}
    l->init = NULL;
    l->total = 0;
    l->last = NULL;
    pthread_mutex_init(&l->listmutex, NULL);

    return l;
}

// es vacío si el nodo inicial no apunta a nada
// Ponerlo como STATIC INT
int 
isempty(List *l){
    
    if(l->init == NULL){
        return 1;
    }
    return 0;
}

// insertar al final de la lista cuando se hace ADDPLAYER
void 
insertatend(List *l, Node *n){
    
    pthread_mutex_lock(&l->listmutex);
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
    pthread_mutex_unlock(&l->listmutex);

}
// lo usamos par imprimir HIGHSCORE
void
printlist(List *l){

    Node *aux = l->init;

    pthread_mutex_lock(&l->listmutex);

    while (aux != NULL){
        printnode(aux);
        aux=aux->next;
    }
    pthread_mutex_unlock(&l->listmutex);

}

// ponerla como STATIC INT
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

// STATIC INT
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
// STATIC INT 
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

//  se usa para DELPLAYER 
void
elimbyname(List *l, char *name){
    int i;
    Node *aux;
    i = 0;
    aux = l->init;

    pthread_mutex_lock(&l->listmutex);

    while(aux != NULL && strcmp(aux->name, name)!= 0){
        aux = aux->next;
        i++;
    }
    // Hemos encontrado el nodo y por lo tanto sabemos su índice
    if(aux != NULL){
        elimbyindex(l,i);
    }
    pthread_mutex_unlock(&l->listmutex);

}

// SE USA EN EL MAIN

Node *
searchbyname(List *l, char *name){

    Node *aux = l->init;

    pthread_mutex_lock(&l->listmutex);

    while(aux != NULL && strcmp(aux->name, name)!= 0){
        aux=aux->next;
    }

    pthread_mutex_unlock(&l->listmutex);

    return aux; 
}

static Node *
_searchbyname(List *l, char *name){

    Node *aux = l->init;

    while(aux != NULL && strcmp(aux->name, name)!= 0){
        aux=aux->next;
    }
    return aux; 
}

// se usará para vaciar la lista
void
emptylist(List *l){
    
    while(!isempty(l)){
        elimfirst(l);
    }
    pthread_mutex_destroy(&l->listmutex);

    free(l);
}

// SE USA EN EL MAIN para cambiar la puntuación en ADDPLAYER
void
changevalue(List *l, char *name, long long value){

    Node *n;
    pthread_mutex_lock(&l->listmutex);

    n = _searchbyname(l,name);
    if(n != NULL){
        if(n->score < value){
            n->score = value;
        }
    }

    pthread_mutex_unlock(&l->listmutex);

}

// **************************************************************************
// ******************* OPERACIONES PARA LOS COMANDOS ************************
void
usage(void)
{
	fprintf(stderr, "usage: ./highscore\n");
	exit(EXIT_FAILURE);
}


int
getnumwords(char *line)
{
	int i;
	int numwords;
	int inword;

	i = 0;
	numwords = 0;
	// inword se inicializa como falso e
	// indica si nos encontramos dentro de una palabra
	inword = 0;
	while (line[i] != '\0') {

		// si el carácter actual no es ninguno de los que buscamos
		// y no estamos dentro de una palabra, aumentamos el número de palabra
		// y decidimios que nos encontramos dentro de una palabra
		if (line[i] != ' ' && line[i] != '\t' ) {

			if(!inword){
				numwords++;
				inword = 1;
			}

		}
		// si el caracter es espacio o tabulador y si ha estado 
		// dentro de una palabra, ponemos que acabamos de salir de ella  
		if (line[i] == ' ' || line[i] == '\t') {
			if(inword){
				inword = 0;

			}
		}
		
		i++;
	}
	return numwords;
}

int
checkcmd(char *cmd){

    int i;

    for(i = 0; i < Ncommnads; i++){

        if(strcmp(cmd, commands[i]) == 0){
            return i;
        }
    }
    return -1;

}

// ASCII:
// A = 65, Z = 90
// a = 97, z = 122
// 0 = 48, 9 = 57
int 
isletter(char letter){
    return ((letter >= 'A' && letter <= 'Z') || (letter >= 'a' && letter <= 'z'));
}
int 
isnumber(char letter){
    return (letter >= '0' && letter <= '9');
}
int 
isname(char *word) {

    while (*word) {

        if(!isletter(*word) && !isnumber(*word)){
            return 0;
        }
        word++;
    }
    // si es un carácter alfanumérico
    return 1; 
}

int 
checkarg(char *arg, int value){

    int valuecmd;
    
    valuecmd = value;
    
    switch (value) {
	// obligatoriamente recibe un nombre con numeros y letras
    case Newplayer:
        if(strcmp(arg, "nocommand") == 0 || !isname(arg)){
            valuecmd = -1;
        }
		break;
    // obligatoriamente recibe un nombre con numeros y letras
	case Delplayer:
        if(strcmp(arg, "nocommand") == 0 || !isname(arg)){
            valuecmd = -1;
        }
		break;
    // no recibe ningún argumento más
    case Highscore:
        if(strcmp(arg, "nocommand") != 0){
            valuecmd = -1;
        }
		break;
    // opcionalmente recibe un nombre con numeros y letras
	case Reset:

        if(strcmp(arg, "nocommand") != 0 && !isname(arg)){
            valuecmd = -1;
        }
        break;

	default:
        valuecmd = -1;
	}

    return valuecmd;
}

int
getkindcommand(Command *cl, char *line){

    char *saveptr;
	char *token;
    //char *aux;

    int valueword;
    int valuecmd;

    cl->command = (char *)malloc(sizeof(char) * LineSz);
    cl->arg = (char *)malloc(sizeof(char) * LineSz);

    if (cl->command == NULL || cl->arg == NULL) {
		errx(EXIT_FAILURE,"Error: dynamic memory cannot be allocated");
	}

    if(getnumwords(line) > 2){
        return -1;
    }


    token = strtok_r(line, " \t" , &saveptr);
    if(token == NULL){
        //free(aux);
        //free(cl->command);
        //free(cl->arg);
        return -1;
    }
    // solo nos quedamos con la primera palabra de la tokenización
    strcpy(cl->command, token);

    // solo nos quedamos con el primero de la tokenización
    valueword = checkcmd(cl->command);

    token = strtok_r(NULL, " \t" , &saveptr);

    if(token == NULL){
        strcpy(cl->arg, "nocommand");
    }else{
        strcpy(cl->arg, token);
    }
    // tengo que comprobar el segundo argumento 
    valuecmd = checkarg(cl->arg, valueword);    

    //free(aux);

    return valuecmd;
}

// *****************************************************************************
// Se usará para que el thread mire si es un número o no
// se hagan las operaciones pertinentes
long long
getnumber(char *str)
{
	int base;
	char *endptr;
	long val;

	base = 10;

	errno = 0;
	val = strtoll(str, &endptr, base);

	// Se comprueban posibles errores 
	if (errno != 0) {
		//err(EXIT_FAILURE, "strtol");
        return -1;
	}
	if (endptr == str) {
		//errx(EXIT_FAILURE, "Error: No digits were found in %s", str);
        return -1;
	}
	// Ahora es necesario comprobar si la string ha sido un número o no
	if (*endptr != '\0') {
		//errx(EXIT_FAILURE, "Error: %s is not a complete number", str);
        return -1;
	}
	return val;
}


char *
getcompletepath(char *path, char *dname)
{

	char *fullpath;
	ssize_t lenpath;
	ssize_t lenname;
	ssize_t lenfull;

	// obtenemos la longitud del path
	lenpath = strlen(path);
	// obtenemos la longitud del nombre de entrada de directorio 
	lenname = strlen(dname);

	// path + '/' + nombre de entrada directorio + '\0' 
	lenfull = lenpath + lenname + 2;

	// comprobar que el malloc no es muy grande 
	if (lenfull > LineSz) {
		fprintf(stderr,"Error: Invalid path size");
        return NULL;
	}
	fullpath = (char *)malloc(sizeof(char) * (lenfull));
	if (fullpath == NULL) {
		fprintf(stderr,"Error: dynamic memory cannot be allocated");
        return NULL;
	}

	strncpy(fullpath, path, lenfull);
	// después del path añadir '/'
	fullpath[lenpath] = '/';
	// strncpy no añade '\0' a final de línea
	fullpath[lenfull - 1] = '\0';
	strncpy(fullpath + lenpath + 1, dname, lenfull - lenpath - 1);
	fullpath[lenfull - 1] = '\0';

	return fullpath;
}

void 
deleteplayer(List *l, char *name){
    // borrará la jugador si existe, incluyendo destruir el hilo (pthread joino pthread_exit),
    // el fifo (unlink o rmfifo) y su existencia de la tabla de punttuaciones (elimbyname) 
    // si no existe lo reportará y seguirá ejecutando
    Node *n;
    char *fullpath;

    n=searchbyname(l, name);
    // significa que existe
    if(n!=NULL){

    
        // borrará al jugador de su tabla de puntuaciones
        elimbyname(l,name);
        fprintf(stderr, "borrar jugador\n");

        if (pthread_join(n->id, NULL) != 0) {
            fprintf(stderr, "Error: joining thread");
        }
        fullpath = getcompletepath("/tmp", name);
        unlink(fullpath);

        //fclose(st);


        free(fullpath);
        //free(args);



        // lo que aparece en delete player
    }else{
        fprintf(stderr, " %s does not exist\n", name);

    }

}
void *
addplayer(void *arg){

    // crea el fifo con ese nombre en /tmp
    // si el nombre ya existe, se considera un error
    // Los errores se reportan y se seguirá leyendo comandos
    
    // Para gestionar las lecturas de cada fifo, se creará un hilo nuevo
    // (uno para cada fifo). 
    // En los fifos se escribirán puntuaciones: cuando lea una línea con un número
    // mayor o igual a 0, si el número es mayor que la puntuación más alta,
    // ese número será su nueva puntuación. EMPIEZAN A 0

    // Si la lectura del fifo contiene algo que no sea una línea con números, se
    // considerará un error y será equivalente a ejecutar delplayer de su nombre 

    // ESQUEMA
    // crea el trhead
    // dentro del thread crea el fifo y espera a ver qué valor recibe

    ThreadArgs *args;

    char *fullpath;
    FILE *st;
    char line[LineSz];
    long long number;
    char *name;
    List *l;
    int id;

    args = (ThreadArgs *)arg;

    name = args->name;
    l = args->list;
    id = args->id;


    fullpath = getcompletepath("/tmp", name);
    if(fullpath != NULL){

        //fprintf(stderr, "la línea resultante es %s\n", fullpath);

        // si el valor es 0: el fichero existe
        if (access(fullpath, F_OK) == 0){
            //free(fullpath);
            //free(args);
            fprintf(stderr, "Error: this file exists\n");
            //pthread_exit((void *)1);

        }else{
            
            fprintf(stderr, "this filedoes not exit\n");

            
            if (mkfifo(fullpath, 0664) < 0){
                free(fullpath);
                free(args);
                fprintf(stderr, "Error: cannot make fifo\n");
                pthread_exit((void *)1);

                //err(EXIT_FAILURE, "cannot make fifo %s", fullpath);
            }

            
            // insertar en la tabla
            // crear el thread
            insertatend(l, createnode(name, 0, id));

            while(1){

                st = fopen(fullpath, "r");
                if (st == NULL) {
                    free(fullpath);
                    free(args);
                    fprintf(stderr, "fopen error\n");
                    pthread_exit((void *)1);
                }
                // st es descriptor de fichero (tipo FILE) 
                // que obtenemos los datos
                while (fgets(line, LineSz, st) != NULL) {

                    line[strlen(line) -1] = '\0';
                }

                number = getnumber(line);
                if(number < 0){

                    fclose(st);
                    deleteplayer(l, name);
                    //HAY QUE MIRAR LO DE FCLOSE 

                    //tratarlo en DELETE PLAYER(pasarle id)
                    //elimbyname(l,name);
                    //fprintf(stderr, "borrar jugador\n");
                    //if (unlink(fullpath) < 0){
                    ////unlink(fullpath);

                    //    err(EXIT_FAILURE, "can't remove %s", fullpath);
                    //}   
                    //fclose(st);
                    //break;
                    //free(fullpath);
                    //free(args);
                    //pthread_exit((void *)1);

                    break; 
                }
                fprintf(stderr, "numero: %lld \n", number);

                // actualizar elemento en la tabla si el actual 
                // es mayor al existente
                changevalue(l,name, number);


                if (ferror(st)) {
                    fclose(st);
                    free(fullpath);
                    free(args);
                    fprintf(stderr, "Error: cannot make fifo\n");
                    pthread_exit((void *)1);
                    //err(EXIT_FAILURE, "read error");
                }
                fclose(st);
            }
        }
        free(fullpath);
    }
    free(args);
    return NULL;
}

void
reset(char *name){
    // si no hay nombre: pone a 0 todas las puntuaciones de todos
    // si hay nombre: pone a 0 la puntuación de ese jugador
    // si no existe se reporta y se continua ejecutando (parecido a changevalue)
}
void 
treatkind(List * l, char *name, int kind){

    pthread_t threads[MaxPlayers];
    ThreadArgs *args;
    int i;

    i = 0;
    switch (kind) {
    case Newplayer:

        if(i < MaxPlayers){

            args = (ThreadArgs *)malloc(sizeof(ThreadArgs));
		    if (args == NULL) {
				errx(EXIT_FAILURE,"Error: dynamic memory cannot be allocated");
		    }

            args->name = name;
            args->list = l;
            args->id = i;
            if (pthread_create
		        (&threads[i], NULL, addplayer, (void *)args) != 0) {
			    fprintf(stderr, "Error creating thread\n");
		    }
            i++;

        }else{
            fprintf(stderr, "Exceded Maximun players\n");
        }
    
        //addplayer(l, name);
        //fprintf(stderr, "soy newplayer: %s \n", name);
		break;
	case Delplayer:
        deleteplayer(l, name);
        fprintf(stderr, "soy delplayer: %s\n", name);
        i--;
		break;
    case Highscore:
        // listará las puntuaciones máximas de cada jugador junto con su nombre,
        // en cualquier orden, con nombre y puntuación separada por 2 puntos (printlist)
        printlist(l);
		break;
	case Reset:
        //reset(name);
        fprintf(stderr, "soy reset: %s \n", name);
        break;

	default:
	}
}


int 
main(int argc, char *argv[]){

    char line[LineSz];
    int c;
    int kind;
    Command cl;
    List *l;

    argc--;
    argv++;

    if(argc != 0){
        usage();
    }

    l = createlist();

    while (fgets(line, LineSz, stdin) != NULL){

        // La línea excede el tamaño máximo
        if (line[strlen(line) - 1] != '\n'){
            fprintf(stderr, "Error: Exceeded path size\n");
            // Limpia el buffer de entrada
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        line[strlen(line) -1] = '\0';

        // Comprobar que el input se trate de algún 
        // comando definido y siga su forma correcta
        kind = getkindcommand(&cl,line);
        // si el comando es incorrecto
        if(kind < 0){
            fprintf(stderr, "Error: Incorrect command\n");
            free(cl.command);
            free(cl.arg);
            continue;
        }

        treatkind(l,cl.arg,kind);

        // estar pendiente de las entradas que reciba y manejarlas
        // el thread UNICAMENTE está pendiente de lo que reciba 
        // si es distinto de numero hay que borrarlo
        free(cl.command);
        free(cl.arg);
    }

    // hacer como un wait escribiendo a cada fifo una string para que acaben
    // hacer un write en el fifo adecuado
    emptylist(l);

    printf("YA ACABAO");

    if(!feof(stdin)){
        errx(EXIT_FAILURE, "eof not reached\n");
    }
    if(ferror(stdin)){
        errx(EXIT_FAILURE, "read error");
    }

    exit(EXIT_SUCCESS);
}