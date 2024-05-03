#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "common.h"
#include "executor.h"


char *builtins[NbuiltIn] = {"cd"};

// 	- builtin
//  - fichero ejecutable del dir de trabajo 
//  - fichero ejecutable de alguno de la variable PATH
int
isbuiltin(char *cmd){

	int i;

	for(i = 0; i < NbuiltIn; i++){

        if(strcmp(cmd, builtins[i]) == 0){
            return 1;
        }
    }
	return 0;
}
int 
ispwd(char *cmd){

	return 0;
}
int
ispath(char *cmd){
	return 0;

}

// setbuitlin
// hacer swicth case e incluir todas las funciones
void  
findtypecommand(CommandLine *cl, char *cmd){

	if(isbuiltin(cmd)){
		fprintf(stderr,"soy built in\n");
		//setbuiltin(cmd)
	}
	if (ispwd(cmd)){
		fprintf(stderr,"soy pwd\n");

		//setpwd()
	}
	if(ispath(cmd)){
		fprintf(stderr,"soy path\n");

		//setpath()
	}

	//if(!isbuiltin(cmd) && !ispwd(cmd) && !ispath(cmd)){
	if(!isbuiltin(cmd)){

		cl->status = FINDERROR;
	}		
}

void
findcommands(CommandLine *cl){
    
	
	// Apartir de aquí podemos hacer:
	// 	mirar el primer elemento de cada array y ver si se trata de: 
		// 	- builtin
		//  - fichero ejecutable del dir de trabajo 
		//  - fichero ejecutable de alguno de la variable PATH

	// Para ello distinguir si se trata de un pipeline o no

	long long j;
	//long long k;

	if(cl->numpipes > 0){

		for(j = 0; j < cl->numcommands; j++){
			//for(k = 0; k < cl->numsubcommands[j]; k++){
			findtypecommand(cl,cl->commands[j][0]);
				//fprintf(stderr,"soy el subcommando: %s\n", cl.commands[j][0]);		
			//}

		}
	}else{

		findtypecommand(cl,cl->words[0]);
		//findtypecommand(c)
	}
}
