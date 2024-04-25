#include <stdio.h>
#include <string.h>

int main() {
    char word[] = "hola|a<o";
    char tokens[100][100]; // Array para almacenar los tokens
    int token_count = 0;
    int word_index = 0;
    int token_index = 0;

    // Recorre la palabra caracter por caracter
    while (word[word_index] != '\0') {
        // Si el caracter actual es '|', termina el token actual y avanza al siguiente
        if (word[word_index] == '|') {
            tokens[token_count][token_index] = '\0'; // Agrega un terminador de cadena al final del token
            token_count++; // Incrementa el contador de tokens
            token_index = 0; // Reinicia el índice del token para el siguiente token
            // Agrega el delimitador '|' como un token adicional
            strcpy(tokens[token_count++], "|");
        } else {
            // Si no es un delimitador, agrega el caracter al token actual
            tokens[token_count][token_index++] = word[word_index];
        }
        // Avanza al siguiente caracter de la palabra
        word_index++;
    }
    // Termina el último token
    tokens[token_count][token_index] = '\0';
    token_count++; // Incrementa el contador de tokens para el último token

    // Imprime los tokens almacenados en el array
    for (int i = 0; i < token_count; i++) {
        printf("%s\n", tokens[i]);
    }

    return 0;
}
