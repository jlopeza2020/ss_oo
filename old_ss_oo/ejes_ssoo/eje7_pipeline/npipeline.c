#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

enum {
    READ = 0,
    WRITE = 1,
};

int execute_command(char *command) {
    char *args[256];
    char *token;
    int i = 0;

    token = strtok(command, " ");
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;

    return execvp(args[0], args);
}

int main(int argc, char *argv[]) {
    int status;
    pid_t waitpid;
    
    if (argc < 2) {
        fprintf(stderr, "Usage: %s cmd1 cmd2 ... cmdN\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int num_commands = argc - 1;
    int **pipes = malloc((num_commands - 1) * sizeof(int *));
    if (pipes == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // Crear los pipes
    for (int i = 0; i < num_commands - 1; i++) {
        pipes[i] = malloc(2 * sizeof(int));
        if (pipes[i] == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    pid_t pid;

    // Ejecutar los comandos
    for (int i = 0; i < num_commands; i++) {
        
        pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) { // Proceso hijo
            if (i != 0) { // Redireccionar la entrada del pipe anterior
                
                if (dup2(pipes[i - 1][READ], STDIN_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }
            if (i != num_commands - 1) { // Redireccionar la salida al pipe actual
                
                if (dup2(pipes[i][WRITE], STDOUT_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            
            // Cerrar los extremos del pipe en el proceso hijo
            for (int j = 0; j < num_commands - 1; j++) {
                close(pipes[j][READ]);
                close(pipes[j][WRITE]);
            }

            // Ejecutar el comando correspondiente
            if (execute_command(argv[i + 1]) == -1) {
                perror("execvp");
                exit(EXIT_FAILURE);
            }
        }
    }

    // ESTAMOS EN LA ZONA DEL PADRE
    
    // Cerrar todos los pipes en el proceso padre
    for (int i = 0; i < num_commands - 1; i++) {
        close(pipes[i][READ]);
        close(pipes[i][WRITE]);
    }

    // Esperar a que todos los procesos hijos terminen
    /*for (int i = 0; i < num_commands; i++) {
        if (wait(NULL) == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }
    }*/
    while ((waitpid = wait(&status)) != -1) {
		if (pid == waitpid) {
			if (!WIFEXITED(status)) {
				exit(EXIT_FAILURE);
			}
		}
	}

    // Liberar la memoria asignada a los pipes
    for (int i = 0; i < num_commands - 1; i++) {
        free(pipes[i]);
    }
    free(pipes);

    return EXIT_SUCCESS;
}
