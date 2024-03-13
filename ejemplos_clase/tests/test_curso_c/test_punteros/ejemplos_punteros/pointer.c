#include <stdio.h>
#include <stdlib.h>
#include <err.h>

int
main(int argc, char *argv[])
{
   int **r; //(r contiene la dir de un puntero) es puntero a puntero 
    int *q;
    int a; //(si es local está en la pila)
    int *p; //(dentro de p va a haber una dirección (cada vez en el tiempo)  de una variable de tipo entero) 

    a = 3;
    p = &a; //saca la dirección de la variable en esa llamada a la función (recordar: si es local, su posición cambia) Da la dirección del primer byte de a 

    printf("%d\n", *p); //(va a la variable a la que apunta p) da 3 

    *p = 28; 
    q = p;//  (el contenido de la variable) 
    printf("%d\n", *q);

    //q = &p; //(error de tipos) 

    r = &p;// (puntero a puntero) (es 4) 
    *r = q; 
    fprintf(stderr,"%d\n",**r);

    **r = 12; //apunta donde apunta r que es modificar el valor de a (12). 
    //traviesas r que va a p y luego atraviesas a 
    printf("%d\n",*q);

  

    exit(EXIT_SUCCESS);
}