#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// punteros a string
void
swapstrings(char **str1, char **str2)
{
	char *temp = *str1;

	*str1 = *str2;
	*str2 = temp;
}

void
sortstrings(char **strings, int n)
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
main(int argc, char *argv[])
{
	int i;

	argc--;
    argv++;
	
	sortstrings(argv, argc);
	/*el argumento 0 es el propio nombre 
	   del programa; por eso, lo omitimos */
	for (i = 0; i < argc; i++) {
		printf("%s\n", argv[i]);
	}

	exit(EXIT_SUCCESS);
}
