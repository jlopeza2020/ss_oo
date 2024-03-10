#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>


enum {
	SourceFile,
	DestinationFile,
	MaxArgs,
};

struct Circulararray{ 

    char *array;
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

FILE *
getfd(char *path, int tipefile)
{
	int isaccessible;
	struct stat sb;
	FILE *fd;

	if (tipefile == SourceFile) {

		// permite comprobar si tenemos permisos para leer un fichero
		errno = 0;
		isaccessible = access(path, R_OK);
		if (isaccessible == -1) {
			err(EXIT_FAILURE, "%s", path);
		}
		// permite comprobar si es un fichero normal: no un enlace simbólico, etc.
		if (lstat(path, &sb) == -1) {
			err(EXIT_FAILURE, "lstat");
		}
		if ((sb.st_mode & S_IFMT) != S_IFREG) {
			errx(EXIT_FAILURE, "%s is not a regular file", path);
		}
		// usar open en modo lectura 
		//fd = open(path, O_RDONLY);
        fd = fopen(path, "r");

		if (fd == NULL) {
			err(EXIT_FAILURE, "fopen");
		}
	} else if (tipefile == DestinationFile) {

		// usar open en modo escritura y sino crear el fichero 
		// con los permisos escritos en el tercer parámetro 
		//fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, mode);
		fd = fopen(path, "w");
        if (fd == NULL) {
			err(EXIT_FAILURE, "fopen");
		}
	}

	return fd;
}

int 
isfull(Circulararray *q){
	return (((q->end + 1) % q->size) == q->front);
}

int
isempty(Circulararray *q){
	return (q->front == -1 && q->end == -1);
}

// en un futuro se encolarán chars no ints
void
enqueue(Circulararray *q, int x)
{
    // si el array está vacío ambos punteros se sitúan 
	// en la posición 0 y se almacena en la posición 
	// del puntero "end" (es 0), el valor de x.
	if(q->front == -1 && q->end == -1){
        q->front = q->end = 0;
        q->array[q->end] = x;

    }else if (isfull(q)){ 
        printf("circular array is full\n");
    }else{
		// si hay algún elemento en el array: 
		// se incrementa la posición de end 
		// y se almacena en ella el valor de x.
        q->end = (q->end + 1) % q->size;
        q->array[q->end] = x;
    }
}

void
dequeue(Circulararray *q)
{   
	if(isempty(q)){
        printf("circular array is empty\n");
        return;
    }
	// cuando front y end se encuentran en la misma posición: 
	// (solo queda un elemento en el array)
	// se fijan a -1 para indicar que está vacío el array
    if (q->front == q->end)
        q->front = q->end = -1;
    else
		// si hay más de un elemento en el array: 
		// se avanza una posición del puntero "front"
        q->front = (q->front + 1) % q->size;

}

// se imprime desde donde se encuentra front hasta end
void
display(Circulararray *q)
{
    int i = q->front;
    
	if(isempty(q)){
        printf("circular array is empty\n");
        return;
    }else{
        printf("[");
        while( i != q->end ){
            printf("%c", q->array[i]);
            i = (i+1) % q->size;
        }
    }
    printf("%c", q->array[q->end]);
    printf("]\n");

}


void 
printgrams(char *srcpath, long size){

    FILE *srcfd;
    Circulararray ca;
    size_t bytesread;
    size_t i;

    //inicializada la estructura de datos
    ca.size = size;
    ca.array = (char *)malloc(sizeof(char)* ca.size);  
    ca.front = -1;
    ca.end = -1;

    srcfd = getfd(srcpath, SourceFile);

    // leer todo el fichero
    // leer
    // ffread lee del stream un número de elementos (size), 
    // los guarda en memoria a  partir de ptr (retorna el numero de
    // elementos leídos). Retona menos elementos que los solicitados o cero
    // cuando llega a final de ficheoro o tiene un error. 
    // Hay que usar feof y ferror: 
    // feof: devuelve un número distinto de 0 si ha llegado a final del fichero
    while(!feof(srcfd)){


        bytesread = fread(ca.array,1, ca.size, srcfd);
        //enqueue(&ca, );
        //fprintf(stderr, "mun bytes leido %ld", bytesread);
        for(i = 0; i < bytesread; i++){
            // añade y se hace display
            //fprintf(stderr, "len  %ld ca.array\n", i);
            enqueue(&ca, ca.array[i]);
            if(ca.size == size){ // si la dimensión es 3 imprimirlo
                display(&ca);
                dequeue(&ca);
            }

            //display(&ca);
        }
        
        /*if(ca.size == size){
            display(&ca);
            dequeue(&ca);

        }*/
         //display(&ca);
        //dequeue(&ca);

        // ferror: devuelve un número distinto de 0 si está establecido. 
        if (ferror(srcfd)) {
            errx(EXIT_FAILURE , "fread error ocurred\n");
        }

        //errx(EXIT_FAILURE , "fread failed");
    }
    
    // usar enqueue 
    // display
    // dequeue ??? 
    //

    fclose(srcfd);
    free(ca.array);
}

int
main(int argc, char *argv[]){

    char *srcpath;
    char *buffersize;
    long ngramsize;

    argc--;
	argv++;
    
    if(argc != MaxArgs){
        usage();
    }

    srcpath = argv[0];
    //srcfd = getfd(srcpath, SourceFile);

    buffersize = argv[1];
    ngramsize = getnumber(buffersize);
    if (ngramsize <= 0) {
		errx(EXIT_FAILURE, "second parameter should be bigger than 0");
	}

    //printf("%p value", (void *)srcfd);

    // en este punto tenemos el fichero de origen abierto 
    // y el tamaño del buffer

    printgrams(srcpath, ngramsize);
    //fread lee dle



    //fclose(srcfd);


	/*
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

    free(ca.array);*/
 

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