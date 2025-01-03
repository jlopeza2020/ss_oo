#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
printargs(char **strings, int n)
{
	int i;

	for (i = 0; i < n; i++) {
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

	sortargs(argv, argc);
	printargs(argv, argc);

	exit(EXIT_SUCCESS);
}
