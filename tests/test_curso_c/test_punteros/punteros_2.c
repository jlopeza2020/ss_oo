#include <stdio.h>

int main() {
    int num = 5;  // Definimos una variable entera num
    int *puntero_num;  // Declaramos un puntero a entero
    int **puntero_puntero_num;  // Declaramos un puntero a puntero a entero
    
    puntero_num = &num;  // Asignamos la dirección de memoria de num al puntero
    puntero_puntero_num = &puntero_num;  // Asignamos la dirección de memoria de puntero_num al puntero a puntero
    
    // Accedemos al valor de la variable num mediante el puntero a puntero
    printf("El valor de num es: %d\n", **puntero_puntero_num);
    
    return 0;
}
