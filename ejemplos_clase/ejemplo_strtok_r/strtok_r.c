#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char *token, *subtoken;
    char *saveptr1, *saveptr2;

    
    argc--;
	argv++;

    if (argc != 3) {
        fprintf(stderr, "Usage: ./strtook_r string delim subdelim\n");
        exit(EXIT_FAILURE);
    }

    // asignamos el primer argumento al string 1
    char *str1 = argv[0];
    for (int j = 1; ; j++, str1 = NULL) {
        token = strtok_r(str1, argv[1], &saveptr1);
        if (token == NULL)
            break;
        printf("%d: %s\n", j, token);

        for (char *str2 = token; ; str2 = NULL) {
            subtoken = strtok_r(str2, argv[2], &saveptr2);
            if (subtoken == NULL)
                break;
            printf(" --> %s\n", subtoken);
        }
    }

    exit(EXIT_SUCCESS);
}
