#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <errno.h>


// estructura de datos para el 
// buffer circular

struct Gram{

    int size;
    char *block; // hacer malloc de su tamaño
    int first; // puntero que apunta al principio
    int last; // puntero que apunta al final
};

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

	// Para distinguir entre success/failure después de la llamada
	errno = 0;
	val = strtol(str, &endptr, base);

	// Se comprueban posibles errores 
	if (errno != 0) {
		err(EXIT_FAILURE, "strtol");
	}

	if (endptr == str) {
		errx(EXIT_FAILURE, "No digits were found");
	}
	// Si llegamos hasta aquí strtol() ha sido capaz de parsear un número
	// Ahora es necesario comprobar si la string ha sido un número o no

	if (*endptr != '\0') {
		errx(EXIT_FAILURE, "is not a complete number");
	}
	return val;
}

int
main(int argc, char *argv[]){

    argc--;
	argv++;

    long ngramsize;

    if(argc != 2){
        usage();
    }
    ngramsize = getnumber(argv[1]);

    if (ngramsize <= 0) {
		errx(EXIT_FAILURE, "second parameter should be bigger than 0");
	}

    printf("tamaño de size %ld\n", ngramsize);

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