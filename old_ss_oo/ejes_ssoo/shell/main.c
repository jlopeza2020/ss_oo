#include "shell.h"
#include "pipes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <err.h>

int 
main(int argc, char *argv[])
{ 
	char string[MAX];  
	char end_string[]="EXIT";
	int end=0,i,segplano=0,get_stdout = dup(1), get_stdin = dup(0), parameters;

	while(end==0) 
    {
        close(WRITE_END);
        dup(get_stdout); // designate the stdoutput, the terminal
        close(READ_END); 
        dup(get_stdin); // designate the stdoutput, the terminal
    	printf("# "); // prints the Prompt
    	scanf("\n%[^\n]",string); // get the whole string 

    	segplano=0;
    	for(i=0;string[i] != '\0'; i++){
        	if(string[i] == '&') {
                string[i] = '\0';
                segplano = 1;
            }
        }
    	end=equal_strings(string,end_string); // if string is EXIT, get out of the while
        parameters = argc -1;
    	command(string,segplano, parameters, argv); // if not, gets into command function
    }
	exit(EXIT_SUCCESS);// end of the program (string 'EXIT' introduced)
	
}