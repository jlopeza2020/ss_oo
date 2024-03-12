#include <stdlib.h> 
#include <stdio.h> 

int
main(int argc, char *argv[])
{
	
    int **r; 

    int a; 
    int *p; 
    int *q; 

    a = 3; 

    p = &a;

    printf("%d\n", *p); // tendrá el valor de 3

    *p = 28;

    printf("%d\n", *p);  // p vale 28

    q = p; // ambos punteros apuntan al mismo sitio

    printf("%d\n", *q); // imprime 28

    // q = &p; // ERROR !!!!! de tipos 

    a = 5;

    
    printf("%d  %d\n", *p, *q);  // los dos punteros imprimen 5 


    r = &p;

    printf("%d\n", **r); // imprimirá 5 

    **r = 12;
    printf("%d\n", **r); // imprimirá 12

	exit(EXIT_SUCCESS);
}