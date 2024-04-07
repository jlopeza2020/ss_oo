#include <stdio.h>
#include <string.h>

void contar_comandos_y_subcomandos(char *comandos[], int longitud) {
    int num_comandos = 0;
    int num_subcomandos = 0;

    for (int i = 0; i < longitud; i++) {
        if (strcmp(comandos[i], "|") == 0) {
            printf("Comando %d tiene %d subcomandos.\n", num_comandos, num_subcomandos);
            num_comandos++;
            num_subcomandos = 0;
        } else {
            num_subcomandos++;
        }
    }

    // Imprimir el último comando
    printf("Comando %d tiene %d subcomandos.\n", num_comandos, num_subcomandos);
}

int main() {
    char *comandos[] = {"cat", "/tmp/q", "|", "wc", "-c", "|", "tail"};
    int longitud = sizeof(comandos) / sizeof(comandos[0]);
    contar_comandos_y_subcomandos(comandos, longitud);

    return 0;
}
