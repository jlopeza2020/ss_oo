#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//ordena los argumentos
void
insertion_sort(int argc, char *argv[])
{
	for (int i = 1; i < argc; i++) {
		for (int j = i; j > 0; j--) {
			//si la posicion actual es menor que la anterior
			//hay que intercambiar las posiciones   
			if (strcmp(argv[j], argv[j - 1]) < 0) {
				char *tmp = argv[j - 1];

				argv[j - 1] = argv[j];
				argv[j] = tmp;
			}
		}
	}
}

//funcion que mira si una palabra tiene vocales y devuelve el numero de vocales 
int
tiene_vocales(int argc, char *argv[], int indice_p)
{
	char vocales[] = "aAeEiIoOuU";
	int long_arr_vocales = strlen(vocales);

	int lon_palabra = strlen(argv[indice_p]);

	//contador de vocales
	int vocal = 0;

	//recorre cada caracter de cada argumento
	for (int indice_c = 0; indice_c < lon_palabra; indice_c++) {
		//compara cada caracter para ver si es una vocal
		for (int pos_arr_v = 0; pos_arr_v < long_arr_vocales;
		     pos_arr_v++) {
			if (argv[indice_p][indice_c] == vocales[pos_arr_v])
				vocal++;
		}
	}
	return vocal;
}

void
imprime_salida(int argc, char *argv[])
{
	//ordena los argumentos
	insertion_sort(argc, argv);
	//recorre los argumentos ordenados
	for (int indice_p = 1; indice_p < argc; indice_p++) {
		int tiene_vocal = tiene_vocales(argc, argv, indice_p);

		//imprime por pantalla las palabras ya ordenadas que tienen vocales
		if (tiene_vocal != 0)
			printf("%s\n", argv[indice_p]);
	}
}

int
main(int argc, char *argv[])
{
	//si se introducen argumentos imprime la salida 
	//y sino, sale por la salida de error
	if (argc != 1) {
		imprime_salida(argc, argv);
		exit(EXIT_SUCCESS);
	} else {
		fprintf(stderr, "usage: ./sortstr string [string ..] \n");
		exit(EXIT_FAILURE);
	}

}
