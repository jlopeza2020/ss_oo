#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parser.h"

void
freememory(CheckInput *checkinput, int numwords){

    int i; 

    // inicializamos cada elemento del array 
    for (i = 0; i < numwords; i++) {
        fprintf(stderr," liberando %s\n",checkinput->words[i]);
        free(checkinput->words[i]);
    }

    free(checkinput->words);
}

void
tokenize(CheckInput *checkinput, char *line, int numwords){

    int i; 
    char *saveptr;
    char *token;

    checkinput->words = (char **)malloc(sizeof(char *)* numwords);
    if (checkinput->words  == NULL) {
		perror("Error: dynamic memory cannot be allocated");
	}

    // inicializamos cada elemento del array 
    for (i = 0; i < numwords; i++) {
        checkinput->words[i] = (char *)malloc(sizeof(char) * MaxWord);
        //checkinput->words[i] = NULL;
        if (checkinput->words[i]  == NULL) {
		    perror("Error: dynamic memory cannot be allocated");
	    }

    }

    i = 0;
    token = strtok_r(line, " \t", &saveptr);
    //for (i = 0; i < numwords; i++){
    strcpy(checkinput->words[i], token); // Copia el token en la memoria asignada
    //checkinput->words[i] = token;
    fprintf(stderr,"token: %s\n",token);
    fprintf(stderr," in estructura token %s\n",checkinput->words[i]);

    i++;
    while (token != NULL && i < numwords) {
        
        //token = strtok_r(line, " \t", &saveptr);
        token = strtok_r(NULL, " \t", &saveptr);
        fprintf(stderr,"token: %s\n",token);
        
        //if (token == NULL)
        //    break;
        strcpy(checkinput->words[i], token); // Copia el token en la memoria asignada
        //checkinput->words[i] = token;
        fprintf(stderr," in estructura token %s\n",checkinput->words[i]);

        i++;
    }
}

int
getnumwords(char *line){

    int i;
    int numwords;
    int inword;
    
    i = 0;
    numwords = 0;
    inword = 0; // es falso
    while(line[i] != '\0'){

        if((line[i] != ' ' && line[i] != '\t') && !inword){
            numwords++;
            inword = 1;
        }

        if((line[i] == ' ' || line[i] == '\t') && inword){
            inword = 0;
        }
        
        i++;
    }
    return numwords;
}
