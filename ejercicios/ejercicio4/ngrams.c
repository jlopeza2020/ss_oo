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

struct Circulararray {

	char *array;
	int front;
	int end;
	long size;
	long szcounter;
};
typedef struct Circulararray Circulararray;

void
usage(void)
{
	fprintf(stderr, "usage: ./ngrams routefile ngramsize\n");
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
isfull(Circulararray *q)
{
	return (((q->end + 1) % q->size) == q->front);
}

int
isempty(Circulararray *q)
{
	return (q->front == -1 && q->end == -1);
}

void
enqueue(Circulararray *q, int x)
{
	// si el array está vacío ambos punteros se sitúan 
	// en la posición 0 y se almacena en la posición 
	// del puntero "end" (es 0), el valor de x.
	if (q->front == -1 && q->end == -1) {
		q->front = q->end = 0;
		q->array[q->end] = x;

	} else if (isfull(q)) {
		printf("circular array is full\n");
		return;
	} else {
		// si hay algún elemento en el array: 
		// se incrementa la posición de end 
		// y se almacena en ella el valor de x.
		q->end = (q->end + 1) % q->size;
		q->array[q->end] = x;
	}
	q->szcounter++;
}

void
dequeue(Circulararray *q)
{
	if (isempty(q)) {
		printf("circular array is empty\n");
		return;
	}
	// cuando front y end se encuentran en la misma posición: 
	// (solo queda un elemento en el array)
	// se fijan a -1 para indicar que está vacío el array
	if (q->front == q->end) {
		q->front = q->end = -1;
	} else {
		// si hay más de un elemento en el array: 
		// se avanza una posición del puntero "front"
		q->front = (q->front + 1) % q->size;
	}
	q->szcounter--;

}

// se imprime desde donde se encuentra front hasta end
void
display(Circulararray *q)
{
	int i = q->front;

	if (isempty(q)) {
		printf("circular array is empty\n");
		return;
	} else {
		printf("[");
		while (i != q->end) {
			printf("%c", q->array[i]);
			i = (i + 1) % q->size;
		}
	}
	printf("%c", q->array[q->end]);
	printf("]\n");

}

void
printgrams(char *srcpath, long size)
{

	FILE *srcfd;
	Circulararray ca;
	char c;

	//inicializada la estructura de datos
	ca.size = size;
	ca.array = (char *)malloc(sizeof(char) * ca.size);
	ca.front = -1;
	ca.end = -1;
	ca.szcounter = 0;

	srcfd = getfd(srcpath, SourceFile);

	// lee hasta final de fichero
	while (fread(&c, sizeof(char), 1, srcfd) != 0) {

		enqueue(&ca, c);
		if (ca.szcounter == size) {
			display(&ca);
			dequeue(&ca);
		}
		// ferror: devuelve un número distinto de 0 si hay error. 
		if (ferror(srcfd)) {
			errx(EXIT_FAILURE, "fread error ocurred\n");
		}
	}

	fclose(srcfd);
	free(ca.array);
}

int
main(int argc, char *argv[])
{

	char *srcpath;
	char *buffersize;
	long ngramsize;

	argc--;
	argv++;

	if (argc != MaxArgs) {
		usage();
	}
	srcpath = argv[0];

	buffersize = argv[1];
	ngramsize = getnumber(buffersize);
	if (ngramsize <= 0) {
		errx(EXIT_FAILURE, "second parameter should be bigger than 0");
	}

	printgrams(srcpath, ngramsize);

	exit(EXIT_SUCCESS);
}
