
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <fcntl.h>

// quitar el usuario de la tabla es complejo 
// hacer un cleanup 
// el principal sale cuando acaben todos 
// no condicienes de carrera, los fifos funcionen correctamente 
// el cleanp ordenado sin condiciones de carrera y demás 

// ******************* DEFINICIONES PARA LA TABLA ****************************
struct Node{
    long long score;
    char *name;
    int id;
    pthread_t thread;
    int running;

    struct Node *next; 
};
typedef struct Node Node;

struct List{

    Node *init;
    int total;
    Node *last; 
    pthread_mutex_t listmutex;

};
typedef struct List List;

// ***************************************************************************
// ***************** DEFINICIÓN PARA COMANDOS ******************************
enum {
    MaxPlayers = 50,
    NameSz = 1024,
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
    pthread_t thread;

};
typedef struct ThreadArgs ThreadArgs;
// ***************************************************************************
// ******************** OPERACIONES PARA EL NODO *****************************
Node *
//createnode(char *name, long long score, int id, FILE *fd,pthread_t thread){
createnode(char *name, long long score, int id,pthread_t thread){

    Node *n = (Node *)malloc(sizeof(Node));
    if (n == NULL) {
		errx(EXIT_FAILURE, "Error: dynamic memory cannot be allocated");
	}

    n->name = (char *)malloc(sizeof(char)*NameSz);
    if (name == NULL) {
		errx(EXIT_FAILURE, "Error: dynamic memory cannot be allocated");
	}

    strcpy(n->name,name);

    n->score = score;
    n->id = id;
    n->thread = thread;
    n->running = 1;
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
        //fprintf(stderr,"valor id:%d\n", aux->id);
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
updateids(List *l, int deletedid) {

    Node *current = l->init;
    while (current != NULL) {
        if (current->id > deletedid) {
            current->id -= 1;
        }
        current = current->next;
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
            updateids(l, 0);
        }else if(index == (l->total -1)){
            elimlast(l);
            updateids(l, l->total -1);
        }else{
            
            // se define un nodo aux que apunta 
            // al principio de la lista
            aux = l->init;
            for(i = 0; i < index; i++){
                // se usa otro nodo pre que se encuentra una posición
                // previa a la de aux
                pre=aux;
                aux=aux->next;
            }
            // cuando llegamos al índice buscado: 
            updateids(l, aux->id);
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

void
resetvalue(List *l, char *name, long long value){

    Node *n;
    pthread_mutex_lock(&l->listmutex);

    n = _searchbyname(l,name);
    if(n != NULL){
        n->score = value;
    }

    pthread_mutex_unlock(&l->listmutex);

}

void
resetallvalues(List *l, long long value){


    Node *aux = l->init;
    pthread_mutex_lock(&l->listmutex);

    while (aux != NULL){
        aux->score  = value;
        aux=aux->next;
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
	// inword (dentro de una palabra) se inicializa como falso
	inword = 0;
	while (line[i] != '\0') {

		// si el carácter actual no es ninguno de los que buscamos
		// y no estamos dentro de una palabra, aumentamos el número de palabra
		// y decidimos que nos encontramos dentro de una palabra
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

    int valueword;
    int valuecmd;

    if(getnumwords(line) > 2){
        return -1;
    }

    token = strtok_r(line, " \t" , &saveptr);
    if(token == NULL){
        return -1;
    }
    // solo nos quedamos con la primera palabra de la tokenización
    strcpy(cl->command, token);

    valueword = checkcmd(cl->command);

    token = strtok_r(NULL, " \t" , &saveptr);

    if(token == NULL){
        strcpy(cl->arg, "nocommand");
    }else{
        strcpy(cl->arg, token);
    }
    // tengo que comprobar el segundo argumento 
    valuecmd = checkarg(cl->arg, valueword); 

    return valuecmd;
}

// *****************************************************************************

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
	if (errno != 0 || endptr == str ||*endptr != '\0') {
        return -1;
	}

	return val;
}

void 
getcompletepath(char *path, char *dname, char *fullpath) {
    
    ssize_t lenpath;
    ssize_t lenname;
    ssize_t lenfull;

    lenpath = strlen(path);
    lenname = strlen(dname);
    lenfull = lenpath + lenname + 2;

    strncpy(fullpath, path, lenfull);
    fullpath[lenpath] = '/';
    fullpath[lenfull - 1] = '\0';
    strncpy(fullpath + lenpath + 1, dname, lenfull - lenpath - 1);
    fullpath[lenfull - 1] = '\0';
}

void 
closethread(char *path) {

    FILE *file;
    size_t bytes_written;
    char data[] = "EXIT\n";

    // Abrir el fichero en modo de escritura
    file = fopen(path, "w");
    if (file == NULL) {
        errx(EXIT_FAILURE, "Error: fopen failed%s\n", path);
    }

    bytes_written = fwrite(data, sizeof(char), sizeof(data), file);
    if (bytes_written != sizeof(data)) {
        fclose(file);
        errx(EXIT_FAILURE, "Error: fwrite failed\n");
    }

    // Cerrar el fichero después de escribir los datos
    if (fclose(file) != 0) {
        errx(EXIT_FAILURE, "Error: fclose failed\n");
    }

    printf("Datos escritos correctamente en el fichero %s\n", path);
}

void
elimbyname(pthread_t thread,List *l, char *name){

    int i;
    Node *aux;
    char fullpath[LineSz];

    i = 0;
    aux = l->init;

    pthread_mutex_lock(&l->listmutex);

    while(aux != NULL && strcmp(aux->name, name)!= 0){
        aux = aux->next;
        i++;
    }
    // Hemos encontrado el nodo y por lo tanto sabemos su índice
    if(aux != NULL){

        getcompletepath("/tmp", name, fullpath);

        closethread(fullpath);
        
        if (unlink(fullpath) != 0) {
            fprintf(stderr, "Error: unlink failed\n");
        }

        if (pthread_join(thread, NULL) != 0) {
            fprintf(stderr, "Error: join failed\n");
		}

        elimbyindex(l,i);

    }
    pthread_mutex_unlock(&l->listmutex);

}


void 
deleteplayer(List *l, char *name, int *id) {

    //pthread_mutex_lock(&l->listmutex);

    char fullpath[LineSz];
    Node *n = searchbyname(l, name);

    // Si el jugador existe
    if (n != NULL) {
        //n->running = 0;

        getcompletepath("/tmp", n->name, fullpath);

        closethread(fullpath);

        //elimbyname(n->thread,l, name); 
        //*id = *id - 1;
    } else {
        fprintf(stderr, "%s does not exist\n", name);
    }
    free(name);
    //pthread_mutex_unlock(&l->listmutex);


}


void 
joindeadthreads(List *l) {

    
    pthread_mutex_lock(&l->listmutex);

    Node *current = l->init;
    Node *previous = NULL;
    char fullpath[LineSz];

    while (current != NULL) {
        if (current->running == 0) {
            Node *temp = current;
            current = current->next;

            getcompletepath("/tmp", temp->name, fullpath);
            if (unlink(fullpath) != 0) {
                fprintf(stderr, "Error: unlink failed\n");
            }
            if (pthread_join(temp->thread, NULL) != 0) {
                fprintf(stderr, "Error: join failed\n");
            }

            // Eliminar el nodo de la lista
            if (previous == NULL) {
                l->init = current;
            } else {
                previous->next = current;
            }

            // Liberar la memoria del nodo eliminado
            free(temp->name);
            free(temp);
        } else {
            previous = current;
            current = current->next;
        }
    }

    pthread_mutex_unlock(&l->listmutex);
}


void *
scoreupdating(void *arg) {

    ThreadArgs *args;
    char *name;
    List *l;
    int id;
    Node * n;

    char line[LineSz];
    long long number;
    FILE *fd;
    char *newline;
    char fullpath[LineSz];
    pthread_t thread;

    args = (ThreadArgs *)arg;

    name = args->name;
    // puntero a la lista
    l = args->list;
    id = args->id;
    thread = args->thread;


    // revisar si hacerlo  dentro del bucle while tanto fopen como fclose 
    // hacerlo también de lectura
    getcompletepath("/tmp", name, fullpath);
    fd = fopen(fullpath, "r+");

    if (fd == NULL) {
        fprintf(stderr, "Error: opening FIFO for player\n");
        free(name);
        free(args);
        pthread_exit((void *)1);
    }

    insertatend(l, createnode(name, 0, id, thread));


    while (fgets(line, LineSz, fd) != NULL) {
        
        if (line[strlen(line) - 1] != '\n') {
            fprintf(stderr, "Exceeded path size\n");
            continue;
        }
        newline = strrchr(line, '\n');
        if (newline != NULL) {
            *newline = '\0';
        }
        number = getnumber(line);
        
        if (number < 0) {
            n = searchbyname(l,name);
            if(n != NULL){
                n->running = 0;
                fprintf(stderr, "running a 0 %d\n", n->running);

            }
            break;
        }
        changevalue(l, name, number);
    }

    if (ferror(fd)) {
        fclose(fd);
        free(name);
        free(args);
        fprintf(stderr, "Error: read error\n");
        pthread_exit((void *)1);
    }

    if (fclose(fd) != 0) {
        free(name);
        free(args);
        fprintf(stderr, "Error: fclose failed\n");
        pthread_exit((void *)1);
    }
    
    //if(strcmp(line, "EXIT") == 0){
    //    fprintf(stderr, "Lo he recibido por delplayer\n");
    //    free(name);
    //    free(args);
    //}else{
    //    fprintf(stderr, "break\n");
    free(name);
    free(args);
        // hacer exit
    //}

    return NULL;
    //pthread_exit(NULL);
}

void 
addplayer(List *l, char *name, int *id) {

    ThreadArgs *args;
    char fullpath[LineSz];

    getcompletepath("/tmp", name, fullpath);
    if (fullpath == NULL) {
        return;
    }
    if (access(fullpath, F_OK) == 0) {
        free(name);
        fprintf(stderr, "Error: this file exists!\n");
        return;
    }

    if (mkfifo(fullpath, 0664) < 0) {
        free(name);
        err(EXIT_FAILURE, "cannot make fifo %s", fullpath);
    }

    if (*id < MaxPlayers) {

        args = (ThreadArgs *)malloc(sizeof(ThreadArgs));

        if(args == NULL) {
            free(name);
            unlink(fullpath);
            errx(EXIT_FAILURE,"Error: dynamic memory cannot be allocated");
        }
        args->name = name;
        args->list = l;
        args->id = *id;
           
        if (pthread_create(&args->thread, NULL, scoreupdating, (void *)args) != 0) {

            free(name);
            free(args);
            unlink(fullpath);
            fprintf(stderr, "Error creating thread\n");
        }

        *id = *id + 1;

    } else {
        fprintf(stderr, "Exceeded Maximum players\n");
        free(name);
        unlink(fullpath);
    }
}


void
reset(List *l, char *name){
    Node *n;

    if(strcmp(name, "nocommand")!= 0){
        
        n=searchbyname(l, name);
        // significa que existe el usuario
        if(n!=NULL){
            resetvalue(l,name,0);
        }else{
            fprintf(stderr, "Error: this user does not exist\n");
        }
    }else{
        resetallvalues(l,0);
    }
}


// TENGO QUE MIRAR LO DE THREADS Y ARGS
int 
main(int argc, char *argv[]){

    char line[LineSz];
    int c;
    int kind;
    Command cl;
    List *l;
    int id;

    id = 0;
    argc--;
    argv++;

    if(argc != 0){
        usage();
    }

    l = createlist();

    while (fgets(line, LineSz, stdin) != NULL){

        if (line[strlen(line) - 1] != '\n'){
            fprintf(stderr, "Error: Exceeded path size\n");
            // Limpia el buffer de entrada
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        line[strlen(line) -1] = '\0';

        cl.command = (char *)malloc(sizeof(char) * LineSz);
        cl.arg = (char *)malloc(sizeof(char) * LineSz);

        if (cl.command == NULL || cl.arg == NULL) {
		    errx(EXIT_FAILURE,"Error: dynamic memory cannot be allocated");
	    }

        kind = getkindcommand(&cl,line);

        switch (kind) {
        case Newplayer:
            free(cl.command);
            addplayer(l, cl.arg, &id);

		    break;
	    case Delplayer:
            free(cl.command);
            deleteplayer(l, cl.arg, &id);

		    break;
        case Highscore:
            printlist(l);
            free(cl.command);
            free(cl.arg);
		    break;
	    case Reset:
            reset(l,cl.arg);
            free(cl.command);
            free(cl.arg);
            break;
	    default:
            fprintf(stderr, "Error: Incorrect command\n");
            free(cl.command);
            free(cl.arg);
            continue;
	    }
        // hacer join de los threads muertos
        joindeadthreads(l);

    }

    // hacer como un wait escribiendo a cada fifo una string para que acaben
    // hacer un write en el fifo adecuado
    emptylist(l);

    fprintf(stderr,"YA ACABAO\n");

    if(!feof(stdin)){
        errx(EXIT_FAILURE, "eof not reached\n");
    }
    if(ferror(stdin)){
        errx(EXIT_FAILURE, "read error");
    }

    exit(EXIT_SUCCESS);
}