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

void 
recursive(char *path){

    DIR *d;
    struct dirent *ent;
    struct stat sb;
    char *fullpath;

    //ssize_t lenpath;
    //ssize_t lenname;
    //ssize_t lenfull;

    d = opendir(path);
    if (d == NULL) {
        err(EXIT_FAILURE, "opendir failed: %s", path);
    }

    // obtenemos la longitud del path
    //lenpath = strlen(path);

    while ((ent = readdir(d)) != NULL) {
        if (ent->d_name[0] != '.') {

            // longitud del nombre de entrada de directorio 
            //lenname = strlen(ent->d_name);

            // longitud total: 
            // path + / + nombre de entrada directorio + \0 
            //lenfull = lenpath + lenname + 2; 

            // comprobar que el malloc no es muy grande 
            /*if (lenfull > MaxPath){
                errx(EXIT_FAILURE, "Invalid path size"); 
            }

            fullpath = (char *)malloc(sizeof(char) * (lenfull));
            if (fullpath == NULL) {
		        errx(EXIT_FAILURE, "Error: dynamic memory cannot be allocated\n");
	        }

            strncpy(fullpath, path, lenfull);
            // después del path añadir '/'
            fullpath[lenpath] = '/';
            // strncpy no añade \0 a final de línea
            fullpath[lenfull - 1] = '\0';
            strncpy(fullpath + lenpath + 1, ent->d_name, lenfull - lenpath - 1);
            fullpath[lenfull - 1] = '\0';*/
            fullpath = getcompletepath(path, ent->d_name);


            if (lstat(fullpath, &sb) < 0) {
                free(fullpath);
                // no debería acabar aquí la función
			    err(EXIT_FAILURE, "lstat");
		    }
            // solo si es directorio o fichero convencional seguirá
            // con el bucle. Omite los directorios
		    if ((sb.st_mode & S_IFMT) == S_IFDIR) {
                recursive(fullpath);
		    }else if ((sb.st_mode & S_IFMT) == S_IFREG){

                printf("%s\n", fullpath);
                // comprobar si es .h o .c
            }

            free(fullpath);
        }
    }
    closedir(d);
}

int 
main(int argc, char *argv[]){

    // preguntar si hace falta malloc
    /*char line[MaxLine];
    
    argc--;
	argv++;

    if (argc != ZeroArgs) {
		usage();
	}*/


    if (argc != 2) {
        errx(EXIT_FAILURE, "Uso: %s dir", argv[0]);
    }

    recursive(argv[1]);


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