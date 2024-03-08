#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

enum {
	SourceFile,
	DestinationFile,
	EnoughArgs = 3,
	MaxArgs,
};

void
usage()
{
	fprintf(stderr,
		"usage: ./copybytes orfile desfile buffersize [bytes copied]\n");
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
getfd(char *path, int tipefile)
{
	int isaccessible;
	struct stat sb;
	int fd;

	// Permisos de lectura-escritura para el propietario, grupo y otros
	mode_t mode = 0666;

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
		fd = open(path, O_RDONLY);
		if (fd == -1) {
			err(EXIT_FAILURE, "open");
		}
	} else if (tipefile == DestinationFile) {

		// usar open en modo escritura y sino crear el fichero 
		// con los permisos escritos en el tercer parámetro 
		fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, mode);
		if (fd == -1) {
			err(EXIT_FAILURE, "open");
		}
	}

	return fd;
}

void
copybytes(int srcfd, int destfd, long buffsize, long copybytesize)
{

	char *buffer;
	int nr;
	int offset;
	long offsetdiff;

	offset = 0;

	// tamaño de una variable entera * (numero de veces que queremos crear la estructura)
	buffer = (char *)malloc(sizeof(char) * buffsize);
	if (buffer == NULL) {
		errx(EXIT_FAILURE,
		     "Error: dynamic memory cannot be allocated\n");
	}
	// lee todo del fd de origen hasta que se acabe el fichero
	while ((nr = read(srcfd, buffer, buffsize)) != 0) {

		// si la lectura es -1 es un error 
		if (nr < 0) {
			free(buffer);
			errx(EXIT_FAILURE, "can't read");
		}
		// se va acumulando el valor de cada lectura 
		// para saber hasta dónde ha leído read
		offset += nr;

		// si existe 4º parámetro
		if (copybytesize >= 0) {
			// si la posición actual es mayor que la objetivo:
			// se ha leído más de lo debido 
			if (offset > copybytesize) {

				// el valor que hay que desplazar del offset que se 
				// encuentra adelantado con respecto al valor objetivo
				// diff = actual - objetivo
				offsetdiff = offset - copybytesize;
				// necesario actualizar el valor actual
				offset -= offsetdiff;
				// el valor de escritura en este caso será el valor que
				// se ha obtenido en la lectura menos la diferencia
				// calculada anteriormente
				nr -= offsetdiff;
			}
		}
		// si write devuelve un número distinto al número de bytes
		// solicitado se considera un error 
		if (write(destfd, buffer, nr) != nr) {
			free(buffer);
			errx(EXIT_FAILURE, "can't write");
		}
		// salir del bucle cuando se haya cumplido el número de 
		// bytes copiados del cuarto parámetro
		if (copybytesize > 0 && offset == copybytesize) {
			break;
		}
	}

	free(buffer);
}

int
main(int argc, char *argv[])
{

	argc--;
	argv++;

	char *srcpath = argv[0];
	char *destpath = argv[1];
	long buffsize;
	long copybytesize;

	int srcfd;
	int destfd;

	switch (argc) {

	case EnoughArgs:
		copybytesize = -1;
		break;
	case MaxArgs:
		copybytesize = getnumber(argv[3]);
		if (copybytesize < 0) {
			errx(EXIT_FAILURE,
			     "fourth parameter should be positive");
		}
		break;
	default:
		usage();
	}

	// se comprueba que el tamaño del buffer sea un número,
	// se obtiene su valor y se comprueba que sea > 0
	buffsize = getnumber(argv[2]);
	if (buffsize <= 0) {
		errx(EXIT_FAILURE, "third parameter should be bigger than 0");
	}

	if (strcmp(srcpath, "-") == 0) {
		srcfd = STDIN_FILENO;
	} else {
		srcfd = getfd(srcpath, SourceFile);
	}

	if (strcmp(destpath, "-") == 0) {
		destfd = STDOUT_FILENO;
	} else {
		destfd = getfd(destpath, DestinationFile);
	}

	copybytes(srcfd, destfd, buffsize, copybytesize);

	close(srcfd);
	close(destfd);

	exit(EXIT_SUCCESS);
}
