#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

enum {
	ZeroArgs,
    MaxPath = 8*1024, // 8K
    MaxLine = 256,
};

struct Sourcefiles{
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

char *getcompletepath(char *path, char *dname){

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
    if (lenfull > MaxPath){
        // NO DEBERÍA SALIR YA 
        errx(EXIT_FAILURE, "Invalid path size"); 
    }

    fullpath = (char *)malloc(sizeof(char) * (lenfull));
    if (fullpath == NULL) {
        // NO DEBERIA SALIR YA 
		errx(EXIT_FAILURE, "Error: dynamic memory cannot be allocated\n");
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
isfile(char *name, char *extension){

    char *value;

    value = strrchr(name, '.');
    if(value != NULL){
        if(strcmp(value, extension) == 0){
            return 1; // exito: son iguales
        }
    }
    return 0;
}

void 
//recursive(char *path, Sourcefiles *infosrc){
processdirectory(char *path, Sourcefiles *infosrc){

    DIR *d;
    struct dirent *ent;
    struct stat sb;
    char *fullpath;

    d = opendir(path);
    if (d == NULL) {
        err(EXIT_FAILURE, "opendir failed: %s", path);
    }

    while ((ent = readdir(d)) != NULL) {
        if (ent->d_name[0] != '.') {

            fullpath = getcompletepath(path, ent->d_name);

            // solo si es directorio o fichero convencional seguirá
            // con el bucle. Omite los enlaces simbólicos
            if (lstat(fullpath, &sb) < 0) {
                free(fullpath);
                // NO DEBERIA SALIR YA 
			    err(EXIT_FAILURE, "lstat");
		    }

		    if ((sb.st_mode & S_IFMT) == S_IFDIR) {
                processdirectory(fullpath, infosrc);
                //processdirectory(infosrc);

		    }else if ((sb.st_mode & S_IFMT) == S_IFREG){

                //printf("%s\n", fullpath);
                if (isfile(ent->d_name, ".c")){
                    infosrc->cfiles++;
                    infosrc->totalbytes += sb.st_size;
                    //fprintf(stderr, "soy fichero .c\n");
                }
                if(isfile(ent->d_name, ".h")){
                    //fprintf(stderr, "soy fichero .h\n");
                    infosrc->hfiles++;
                    infosrc->totalbytes += sb.st_size;
                }
            }

            free(fullpath);
        }
    }
    closedir(d);
}

int 
main(int argc, char *argv[]){

    // preguntar si hace falta malloc porque si
    // se pasa de tamaño hce una segunda lectura el programa por si solo 
    char line[MaxLine];
    Sourcefiles infodir;

    argc--;
	argv++;

    if (argc != ZeroArgs) {
		usage();
	}


    /*recursive(argv[1], &infosrc);

    infosrc.path = argv[1];

    printf("%s\t%ld\t%ld\t%lld\n", infosrc.path, infosrc.cfiles, infosrc.hfiles, infosrc.totalbytes);
    */

    while(fgets(line, MaxLine, stdin) != NULL){
        printf("line: %s\n", line);

        line[strcspn(line, "\n")] = 0;

        infodir.path = line;
        infodir.cfiles = 0;
        infodir.hfiles = 0;
        infodir.totalbytes = 0;

        //path[strcspn(path, "\n")] = 0;

        processdirectory(line, &infodir);
        printf("%s\t%ld\t%ld\t%lld\n", infodir.path, infodir.cfiles, infodir.hfiles, infodir.totalbytes);

        // se crea una estructura de datos
        // cada vez que se accede a una entrada de directorio
        // Tratar los errores y ver el valor final que tiene el proceso
        // usar funciones que controlen errno
    }
    if(!feof(stdin)){
        errx(EXIT_FAILURE, "eof not reached\n");
    }

    exit(EXIT_SUCCESS);
}