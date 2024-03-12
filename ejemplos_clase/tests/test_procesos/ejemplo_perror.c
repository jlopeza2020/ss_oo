#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main() {
    int dividend = 10;
    int divisor = 0;

    if (divisor == 0) {
        perror("Error: Division by zero");
        return EXIT_FAILURE;
    }
    printf("Result: %d\n", dividend / divisor);
    return EXIT_SUCCESS;
}
