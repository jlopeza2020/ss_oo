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
    MaxPath = 1024,
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


void 
recursive(char *path){

    DIR *d;
    struct dirent *ent;
    struct stat sb;
    // Usar malloc
    //char fullpath[MaxPath];
    char *fullpath;
    ssize_t lenpath;
    ssize_t lenname;
    ssize_t lenfull;

    d = opendir(path);
    if (d == NULL) {
        err(EXIT_FAILURE, "opendir failed: %s", path);
    }

    // obtenemos la longitud del path
    lenpath = strlen(path);
    // copiamos como mucho MaxLine de path (origen) a fullpath (destino) 
    //strncpy(fullpath, path, MaxPath);
    //fullpath[lenpath] = '/'; // Agregar barra inclinada al final del path
    //fullpath[MaxPath - 1] = '\0'; // Asegurarse de que el búfer esté terminado con nulo


    while ((ent = readdir(d)) != NULL) {
        if (ent->d_name[0] != '.') {

            //necesitamos añadirle al path el nombre de la entrada de directorio
            lenname = strlen(ent->d_name);

            lenfull = lenpath + lenname; 

            if (lenfull > MaxPath){
                errx(EXIT_FAILURE, "Invalid path size"); 
            } // Corregido: se cambió bufsize a t->bufsize

            fullpath = (char *)malloc(sizeof(char) * (lenfull + 2));
            if (fullpath == NULL) {
		        errx(EXIT_FAILURE, "Error: dynamic memory cannot be allocated\n");
	        }


            /*strncpy(fullpath, path, lenfull);
            fullpath[lenpath] = '/';
            strncpy()*/

            //strcat(strcpy(fullpath, path), '/');
            strcpy(fullpath, path);

            // después del path añadir '/'
            fullpath[lenpath] = '/';
			strcat(fullpath, ent->d_name);

            //fprintf(stderr, "prev %s\n", fullpath);


        
            //strncpy(fullpath + lenpath + 1, ent->d_name, MaxPath - lenpath - 1);
            // fullpath[MaxPath - 1] = '\0'; // Asegurarse de que el búfer esté terminado con nulo


            if (lstat(fullpath, &sb) < 0) {
			    err(EXIT_FAILURE, "lstat");
		    }
            // solo si es directorio o fichero convencional seguirá
            // con el bucle 
		    if ((sb.st_mode & S_IFMT) == S_IFDIR) {
			    //errx(EXIT_FAILURE, "%s is not a regular file", path);
                // hay que ir añadiendo las barras 
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

    

    }
    if(!feof(stdin)){
        errx(EXIT_FAILURE, "eof not reached");
    }*/


    exit(EXIT_SUCCESS);
}