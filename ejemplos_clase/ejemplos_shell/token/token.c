void 
handlespecialchars(CommandLine *cl, char *word, int *pos) {
    
    int word_index;
    int token_index;
	//int isfirst;

	word_index = 0;
    token_index = 0;
	//isfirst = 1; 


	//fprintf(stderr, "pos: %d\n", *pos);
    // Recorre la palabra caracter por caracter
    while (word[word_index] != '\0') {
        // Si el caracter actual es '|', termina el token actual y avanza al siguiente
        if (word[word_index] == '|') {
            
			cl->words[*pos][token_index] = '\0'; // Agrega un terminador de cadena al final del token
            *pos = *pos+1; // Incrementa el contador de tokens
            token_index = 0; // Reinicia el índice del token para el siguiente token
            // Agrega el delimitador '|' como un token adicional
            //*pos = *pos +1;
			strcpy(cl->words[*pos], "|"); // se añade también en /0
			*pos = *pos +1;
    
		} else {
            // Si no es un delimitador, agrega el caracter al token actual
            cl->words[*pos][token_index++] = word[word_index];
        }
        // Avanza al siguiente caracter de la palabra
        word_index++;
    }
	// Termina el último token si la palabra no termina con '|'
    if (token_index > 0) {
    	cl->words[*pos][token_index] = '\0';
    	*pos = *pos +1; // Incrementa el contador de tokens para el último token
	}
}

