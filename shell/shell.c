#include <stdlib.h>
#include <stdio.h>
#include <string.h>


enum {
	ZeroArgs,
	MaxLine = 256,
};

void
usage(void)
{
	fprintf(stderr, "usage: ./shell\n");
	exit(EXIT_FAILURE);
}


// ¿este nombre es correcto?
int
getnumwords(char *line){

    int i;
    int numwords;
    int inword;
    
    //printf("%s\n",line);

    i = 0;
    numwords = 0;
    inword = 0; // es falso

    // imprimimos hasta final de la línea 
    // ya que fgets devuelve la string con un \0
    while(line[i] != '\0'){

        
        // imprimimos caracter a carácter 
        //printf("%c\n", line[i]);

        if((line[i] != ' ' && line[i] != '\t') && !inword){
            // no almacenamos dichos valores
            numwords++;
            inword = 1;
        }

        if((line[i] == ' ' || line[i] == '\t') && inword){
            // no almacenamos dichos valores
            inword = 0;
        }
        
        i++;
    }
    return numwords;
}

    //token = strtok_r(str1, argv[1], &saveptr1);

int 
main(int argc, char *argv[]){

    char line[MaxLine];
    int numwords;

    argc--;
	argv++;

    if (argc != ZeroArgs) {
		usage();
	}

    printf("Shell made by Julia López Augusto\n");
	printf("Write EXIT to leave\n");


    while (1) {

        if(fgets(line, MaxLine, stdin) == NULL){
            exit(EXIT_SUCCESS);
        }
        
        //fgets(line, MaxLine, stdin);

        // elimina el salto de línea al final
		line[strcspn(line, "\n")] = 0;

    
        // tokenizar la string
        numwords = getnumwords(line);

        printf("esta string tiene %d palabras\n", numwords);

        // if() si la linea introducida es EXIT, salir de la terminal

        /*if (feof(stdin)) {
            // cerrar todo lo necesario y salir
            exit(EXIT_SUCCESS);
        }*/

    

    }



    exit(EXIT_SUCCESS);
}