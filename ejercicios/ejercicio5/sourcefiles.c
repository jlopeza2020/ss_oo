#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

enum {
	ZeroArgs,
	MaxPath = 16 * 1024, // 16K
	MaxLine = 4 * 1024, // 4K
};

struct Sourcefiles {
	char *path;
	long cfiles;
	long hfiles;
	long long totalbytes;
};
typedef struct Sourcefiles Sourcefiles;

void
usage(void)
{
	fprintf(stderr, "usage: ./sourcefiles\n");
	exit(EXIT_FAILURE);
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
	// longitud del nombre de entrada de directorio 
	lenname = strlen(dname);

	// path + / + nombre de entrada directorio + \0 
	lenfull = lenpath + lenname + 2;

	// comprobar que el malloc no es muy grande 
	if (lenfull > MaxPath) {
		perror("Invalid path size");
	}

	fullpath = (char *)malloc(sizeof(char) * (lenfull));
	if (fullpath == NULL) {
		perror("Error: dynamic memory cannot be allocated");
	}

	strncpy(fullpath, path, lenfull);
	// después del path añadir '/'
	fullpath[lenpath] = '/';
	// strncpy no añade \0 a final de línea
	fullpath[lenfull - 1] = '\0';
	strncpy(fullpath + lenpath + 1, dname, lenfull - lenpath - 1);
	fullpath[lenfull - 1] = '\0';

	return fullpath;
}

int
isfile(char *name, char *extension)
{

	char *value;

	value = strrchr(name, '.');
	if (value != NULL) {
		if (strcmp(value, extension) == 0) {
			return 1;	// exito: son iguales
		}
	}
	return 0;
}

void
processdirectory(char *path, Sourcefiles *infosrc)
{

	DIR *d;
	struct dirent *ent;
	struct stat sb;
	char *fullpath;

	d = opendir(path);
	if (d == NULL) {
		perror("opendir failed");
	} else {

		while ((ent = readdir(d)) != NULL) {
			if (ent->d_name[0] != '.') {

				fullpath = getcompletepath(path, ent->d_name);

				// solo si es directorio o fichero convencional seguirá
				// con el bucle. Omite los enlaces simbólicos
				if (lstat(fullpath, &sb) < 0) {
					free(fullpath);
					perror("lstat");
				}

				if ((sb.st_mode & S_IFMT) == S_IFDIR) {
					processdirectory(fullpath, infosrc);

				} else if ((sb.st_mode & S_IFMT) == S_IFREG) {

					if (isfile(ent->d_name, ".c")) {
						infosrc->cfiles++;
						infosrc->totalbytes +=
						    sb.st_size;
					}
					if (isfile(ent->d_name, ".h")) {
						infosrc->hfiles++;
						infosrc->totalbytes +=
						    sb.st_size;
					}
				}

				free(fullpath);
			}
		}
		closedir(d);
	}
}

int
main(int argc, char *argv[])
{
	char line[MaxLine];
	char *newline;
	Sourcefiles infodir;


	errno = 0;

	argc--;
	argv++;

	if (argc != ZeroArgs) {
		usage();
	}

	while (fgets(line, MaxLine, stdin) != NULL) {

		if (line[strlen(line) - 1] == '\n') {

            // elimina el salto de línea al final
			//line[strcspn(line, "\n")] = 0;
			newline = strrchr(line, '\n'); 

    		if (newline != NULL) {
				// donde apunta newline poner un /0
        		*newline  = '\0';
   		 	}

			// inicializando la estructura de datos
			infodir.path = line;
			infodir.cfiles = 0;
			infodir.hfiles = 0;
			infodir.totalbytes = 0;

			processdirectory(line, &infodir);
			printf("%s\t%ld\t%ld\t%lld\n", infodir.path, infodir.cfiles,
		       infodir.hfiles, infodir.totalbytes);

        } else {
            warnx("Exceeded path size");

		}
	}
	if (!feof(stdin)) {
		errx(EXIT_FAILURE, "eof not reached\n");
	}

	exit(errno);
}
