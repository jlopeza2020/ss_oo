#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void
usage(void)
{
	fprintf(stderr, "Env variable is defined but its value is not forward either backward\n");
	exit(EXIT_FAILURE);
}
// Punteros a string
void
swapstrings(char **str1, char **str2)
{
	char *temp = *str1;

	*str1 = *str2;
	*str2 = temp;
}

void
sortargs(char **strings, int n)
{
	int i;
	int j;

	for (i = 1; i < n; i++) {
		for (j = 0; j < i; j++) {
			/*Intercambiar valores cuando s1 > s2 así,
			   el orden tiene que ir de menor a mayor 
			   debido a la tabla ASCII */
			if (strcmp(strings[j], strings[i]) > 0) {
				swapstrings(&strings[j], &strings[i]);
			}
		}
	}
}

void
printforwardargs(char **strings, int n)
{
	int i;

	for (i = 0; i < n; i++) {
		printf("%s\n", strings[i]);
	}
}

void
printbackwardargs(char **strings, int n)
{
	int i;

	for (i = n-1; i >= 0; --i) {
		printf("%s\n", strings[i]);
	}
}

int
main(int argc, char *argv[])
{
	char *varenv;
    char *forward;
    char *backward;
    
	// Omitimos primer argumento
	argc--;
	argv++;

    forward = "forward";
    backward = "backward";

	sortargs(argv, argc);

    varenv = getenv("DIRECTION");
    /*if (varenv == NULL){ 
        printf("soy NULL\n");
    }*/
    //fprintf(stderr,"%s\n", varenv); // eliminar

    if(varenv == NULL || strcmp(varenv, forward) == 0)
    {
        printforwardargs(argv, argc);

    }else if(strcmp(varenv, backward) == 0)
    {
        printbackwardargs(argv, argc);

    }else{
		usage();
	}
	//printforwardargs(argv, argc);
    //printbackwardargs(argv, argc);

    // si está definida o no es forward o backward -> dar un error -> usage

	exit(EXIT_SUCCESS);
}
