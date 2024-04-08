#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE_LENGTH 1024

void print_usage() {
    printf("usage: pspids PID0 PID1\n");
}

void print_error(const char *msg) {
    printf("error: %s\n", msg);
    print_usage();
}

int main(int argc, char *argv[]) {
    // Check number of arguments
    if (argc != 3) {
        print_usage();
        return 1;
    }

    // Convert arguments to integers
    long pid0 = strtol(argv[1], NULL, 10);
    long pid1 = strtol(argv[2], NULL, 10);

    // Check if arguments are positive numbers
    if (pid0 <= 0 || pid1 <= 0) {
        print_error("PIDs should be positive numbers");
        return 1;
    }

    // Check if pid1 is greater than or equal to pid0
    if (pid1 < pid0) {
        print_error("PID1 should be greater or equal than PID0");
        return 1;
    }

    // Create pipe
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    // Fork a new process
    pid_t process_id = fork();

    if (process_id == -1) {
        // Fork failed
        perror("fork");
        return 1;
    } else if (process_id == 0) {
        // Child process

        // Redirect stdout to pipe
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        // Execute ps command
        char *args[] = {"ps", "-eo", "%p %U %x %c", NULL};
        execv("/bin/ps", args);
        perror("execv");
        exit(EXIT_FAILURE);
    } else {
        // Parent process

        // Close unused write end of the pipe
        close(pipefd[1]);

        // Read from pipe and filter results
        char line[MAX_LINE_LENGTH];
        int count = 0;
        FILE *pipe_read = fdopen(pipefd[0], "r");
        if (pipe_read == NULL) {
            perror("fdopen");
            return 1;
        }
        while (fgets(line, MAX_LINE_LENGTH, pipe_read) != NULL) {
            if (count > 0) { // Skip the header
                long pid;
                char user[MAX_LINE_LENGTH];
                char time[MAX_LINE_LENGTH];
                char command[MAX_LINE_LENGTH];
                if (sscanf(line, "%ld %s %s %s", &pid, user, time, command) == 4) {
                    if (pid >= pid0 && pid <= pid1) {
                        printf("%s", line);
                    }
                }
            }
            count++;
        }

        // Close read end of the pipe
        fclose(pipe_read);

        // Wait for the child process to finish
        wait(NULL);
    }

    return 0;
}
