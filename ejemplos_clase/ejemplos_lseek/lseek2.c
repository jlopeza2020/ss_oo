#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define MAX_BUFFER_SIZE 1024

void copy_bytes(const char *src_path, const char *dest_path, int buffer_size, int bytes_to_copy) {
    int src_fd, dest_fd;
    char buffer[MAX_BUFFER_SIZE];
    ssize_t bytes_read, bytes_written;
    off_t offset = 0;

    // Open source file
    if (strcmp(src_path, "-") == 0)
        src_fd = STDIN_FILENO;
    else
        src_fd = open(src_path, O_RDONLY);
    if (src_fd == -1) {
        perror("Error opening source file");
        exit(EXIT_FAILURE);
    }

    // Open destination file
    if (strcmp(dest_path, "-") == 0)
        dest_fd = STDOUT_FILENO;
    else
        dest_fd = open(dest_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (dest_fd == -1) {
        perror("Error opening destination file");
        exit(EXIT_FAILURE);
    }

    // Set offset if bytes_to_copy specified
    //if (bytes_to_copy > 0)
    //    lseek(src_fd, bytes_to_copy, SEEK_SET);

    // Copy bytes
    while ((bytes_read = read(src_fd, buffer, buffer_size)) > 0) {
        bytes_written = write(dest_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            perror("Error writing to destination file");
            exit(EXIT_FAILURE);
        }
        fprintf(stderr, "bytes_read  %ld", offset);
        offset += bytes_read;
        if (bytes_to_copy > 0 && offset >= bytes_to_copy)
            break;
    }

    // Close files
    if (src_fd != STDIN_FILENO)
        close(src_fd);
    if (dest_fd != STDOUT_FILENO)
        close(dest_fd);
}

int main(int argc, char *argv[]) {
    if (argc < 4 || argc > 5) {
        fprintf(stderr, "Usage: %s <source_path> <dest_path> <buffer_size> [bytes_to_copy]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *src_path = argv[1];
    const char *dest_path = argv[2];
    int buffer_size = atoi(argv[3]);
    int bytes_to_copy = -1;

    if (argc == 5)
        bytes_to_copy = atoi(argv[4]);

    if (buffer_size <= 0) {
        fprintf(stderr, "Buffer size must be a positive integer\n");
        exit(EXIT_FAILURE);
    }

    copy_bytes(src_path, dest_path, buffer_size, bytes_to_copy);

    return 0;
}
