#include <stdlib.h>
#include <stdio.h>

// Imprime el valor en binario 
void 
binprint(char n) // char es un entero con signo de 1 byte 
{
    uint un; // hacer operaciones bit a bit donde meto el carácter
    uint msk; // haré un and del entero que quiero imprimir 
    un = n;

    msk = 1 << ((8*sizeof n) -1); // cada vez que ese and sea true, imprimo un 1 y sino un 0 

    while(msk){
        if(un&msk)
            printf("%d", 1);
        else
            printf("%d", 0);
        msk >>=1; // desplazo el bit a la derecha
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    char c;

    printf("9 -> ");
    binprint(9);  // bin: 00001001

    printf("7 -> ");
    binprint(7);  // bin: 00000111

    printf("7|9 -> "); // OR bit a bit: si alguno de los 2 es 1, será 1
    binprint(7|9);   
                //  9 -> 00001001
                //  7 -> 00000111
              //  9|7 -> 00001111
  
    printf("7&9 -> "); // AND bit a bit: si los 2 son 1, será 1
    binprint(7&9);
                //  9 -> 00001001
                //  7 -> 00000111
              //  9&7 -> 00000001

    printf("7^3 -> "); // XOR bit a bit: si los 2 son distintos, será 1
    binprint(7^3);

             //     3 -> 00000011
                //  7 -> 00000111
              //  3^7 -> 00000100

    printf("7^1 > ");
    binprint(7^1);
             //     1 -> 00000001
                //  7 -> 00000111
              //  1^7 -> 00000110

    printf("9>>1 -> "); // se le mete 1 bit a 0 por el lado izq
    binprint(9>>1);
                //  9    -> 00001001
                //  9>>1 -> 000001001
           // finalmente -> 00000100

    printf("9<<2 -> "); // se le mete 2 bit a 0 por el lado derecho
    binprint(9<<2);
                //  9    ->   00001001
                //  9<<2 -> 0000100100
           // finalmente ->   00100100

    printf("0xff -> "); // es un valor en hexadecimal
    binprint(0xff); 
            // bin: 11111111    

    c = 0xff;
    printf("0xff>>2 -> ");
    binprint(c>>2);
            // 0xff>>2 -> 11111111 
            // ERROR!! Eso ocurre al ser char con signo

    printf("0xff>>2 -> ");
    binprint(0xff>>2);
            // 0xff>>2 -> 00111111 
            // Devuelve el valor deseado

    exit(0);
}