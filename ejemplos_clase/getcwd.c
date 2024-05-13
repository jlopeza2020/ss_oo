#include <stdio.h>
#include <unistd.h>

#define MAX_PATH_LENGTH 4096 // Tamaño máximo de la ruta del directorio

int main() {
    char cwd[MAX_PATH_LENGTH]; // Buffer para almacenar la ruta del directorio

    // Obtener el directorio de trabajo actual
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Directorio actual: %s\n", cwd);
    } else {
        perror("getcwd() error");
        return 1;
    }

    return 0;
}
