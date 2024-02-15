#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h> // Include string.h for strerror()
#include <err.h>

int main() {
    // Example 1: Using perror()
    FILE *file1 = fopen("nonexistent_file.txt", "r");
    if (file1 == NULL) {
        perror("Error opening file");
        // Output: "Error opening file: No such file or directory"
    }
    
    // Example 2: Using strerror()
    FILE *file2 = fopen("nonexistent_file.txt", "r");
    if (file2 == NULL) {
        fprintf(stderr, "Error opening file: %s\n", strerror(errno));
        // Output: "Error opening file: No such file or directory"
    }
    
    // Example 3: Using warn()
    FILE *file3 = fopen("nonexistent_file.txt", "r");
    if (file3 == NULL) {
        warn("Error opening file");
        // Output: "Error opening file: No such file or directory"
    }
    
    return 0;
}
