#include <stdio.h>
#include <string.h>

int main() {
    char texto[] = "echo|";
    char resultado[20][20]; // Ajusta el tamaño según tus necesidades
    char *token, *saveptr;

    // Tokenizamos la cadena original usando '|' y strtok_r
    token = strtok_r(texto, "|", &saveptr);

    // Verificamos si hay un token y si es "echo"
    if (token != NULL && strcmp(token, "echo") == 0) {
        // Almacenamos el primer token en la primera posición del resultado
        strcpy(resultado[0], token);

        // Obtenemos el segundo token
        token = strtok_r(NULL, "|", &saveptr);

        // Verificamos si hay un segundo token
        if (token != NULL) {
            // Almacenamos el segundo token en la segunda posición del resultado
            strcpy(resultado[1], token);

            // Concatenamos ", " al primer token
            strcat(resultado[0], ", ");

            printf("Texto original: %s\n", texto);
            printf("Resultado: [%s, %s]\n", resultado[0], resultado[1]);
        } else {
            printf("Error: No se encontró el segundo token.\n");
            return 1;
        }
    } else {
        printf("Error: No se encontró el token 'echo'.\n");
        return 1;
    }

    return 0;
}
