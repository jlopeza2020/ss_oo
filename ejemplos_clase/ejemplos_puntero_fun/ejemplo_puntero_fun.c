#include <stdio.h>

void prplus(int a)
{
    printf("%d\n", a + 1); // Corregido: Cambiado printff a printf y añadido el formato para el entero.
}

int main()
{
    void (*printsome)(int); // Corregido: La declaración de printsome debe ser dentro de la función main.
    printsome = prplus;
    printsome(3);
    return 0;
}
