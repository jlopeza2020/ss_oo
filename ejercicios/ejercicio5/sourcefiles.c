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
recursive(char *path, Sourcefiles *srcinfo){

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
                recursive(fullpath, srcinfo);
		    }else if ((sb.st_mode & S_IFMT) == S_IFREG){

                printf("%s\n", fullpath);
                if (isfile(ent->d_name, ".c")){
                    srcinfo->cfiles++;
                    srcinfo->totalbytes += sb.st_size;
                    fprintf(stderr, "soy fichero .c\n");
                }
                if(isfile(ent->d_name, ".h")){
                    fprintf(stderr, "soy fichero .h\n");
                    srcinfo->hfiles++;
                    srcinfo->totalbytes += sb.st_size;

                    // se cumple que sea un fichero .h o .c y hay que añadirle
                    // los bytes correspondientes el numero de ficheros
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
    /*char line[MaxLine];
    
    argc--;
	argv++;

    if (argc != ZeroArgs) {
		usage();
	}*/
    Sourcefiles info;

    if (argc != 2) {
        errx(EXIT_FAILURE, "Uso: %s dir", argv[0]);
    }

    info.cfiles = 0;
    info.hfiles = 0;
    info.totalbytes = 0;

    recursive(argv[1], &info);

    info.path = argv[1];

    printf("%s\t%ld\t%ld\t%lld\n", info.path, info.cfiles, info.hfiles, info.totalbytes);


    /*while(fgets(line, MaxLine, stdin) != NULL){
        printf("line: %s\n", line);


    // se crea una estructura de datos
    // cada vez que se accede a una entrada de directorio
    // Tratar los errores y ver el valor final que tiene el proceso
    // usar funciones que controlen errno
    

    }
    if(!feof(stdin)){
        errx(EXIT_FAILURE, "eof not reached");
    }*/


    exit(EXIT_SUCCESS);
}