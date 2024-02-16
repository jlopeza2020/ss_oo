#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void
usage(void)
{
	fprintf(stderr, "usage: eje0 string [string ...] \n");
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
int
checkifvowels(char *string)
{
	
	int j;
    int k;
	int hasvowel;

	hasvowel = 0;

	char vocales[] = "aAeEiIoOuU";

	// recorremos el array de vocales
    for (j = 0; j < strlen(string); j++){
        for(k = 0; k < strlen(vocales); k++){

            if(vocales[k] == string[j]){
				return ++hasvowel;                    
            }
        }
    }

	return hasvowel;

}

void
printargs(char **strings, int n)
{
	int i;

	for (i = 0; i < n; i++) {
		if(checkifvowels(strings[i]) != 0)
			printf("%s\n", strings[i]);
	}
}

int
main(int argc, char *argv[])
{
	/*Omitimos el primer argumento: 
	   es la invocación al propio programa */
	argc--;
	argv++;

	if(argc != 0)
	{
		sortargs(argv, argc);
		printargs(argv, argc);
	}else
		usage();
		
	exit(EXIT_SUCCESS);
}
