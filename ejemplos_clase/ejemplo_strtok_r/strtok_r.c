#include <stdio.h>
#include <stdbool.h>

int count_words(char *line) {
    int num_words = 0;
    bool in_word = false;  // Variable para rastrear si estamos dentro de una palabra

    // Iteramos sobre cada carácter de la cadena
    for (int i = 0; line[i] != '\0'; i++) {
        // Si el carácter actual no es un separador y no estamos en una palabra,
        // aumentamos el contador de palabras y marcamos que estamos dentro de una palabra.
        if ((line[i] != ' ' && line[i] != '\t') && !in_word) {
            num_words++;
            in_word = true;
        } 
        // Si el carácter actual es un separador y estamos dentro de una palabra,
        // marcamos que ya no estamos dentro de una palabra.
        else if ((line[i] == ' ' || line[i] == '\t') && in_word) {
            in_word = false;
        }
    }

    return num_words;
}

int main() {
    char line[] = "Esto es una cadena de ejemplo uh7y77     tt";
    int num_words = count_words(line);
    printf("Número de palabras: %d\n", num_words);
    return 0;
}
