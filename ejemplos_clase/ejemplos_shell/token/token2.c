#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MaxWord 256

char **tokenpath(char *path, long long times) {
    long long i;
    char *saveptr;
    char *token;
    char **tokens;

    tokens = (char **)malloc(sizeof(char *) * (times + 1));
    if (tokens == NULL) {
        perror("Error: dynamic memory cannot be allocated");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < times + 1; i++) {
        tokens[i] = (char *)malloc(sizeof(char) * MaxWord);
        if (tokens[i] == NULL) {
            perror("Error: dynamic memory cannot be allocated");
            exit(EXIT_FAILURE);
        }
    }

    i = 0;

    token = strtok_r(path, ":", &saveptr);
    while (token != NULL && i < times) {
        strcpy(tokens[i], token);
        i++;
        token = strtok_r(NULL, ":", &saveptr);
    }

    // Asegurarse de que el último elemento de tokens sea NULL
    tokens[i] = NULL;

    return tokens;
}

int main() {
    char ruta[] = "/opt/ros/humble/bin:/home/juloau/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/snap/bin";
    char **tokens = tokenpath(ruta, 20); // Cambia el segundo parámetro según sea necesario

    // Imprimir los tokens
    int j = 0;
    while (tokens[j] != NULL) {
        printf("Token %d: %s\n", j, tokens[j]);
        j++;
    }

    // Liberar memoria
    for (int k = 0; tokens[k] != NULL; k++) {
        free(tokens[k]);
    }
    free(tokens);

    return 0;
}
