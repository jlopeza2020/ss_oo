#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_NAME_LENGTH 20
#define FIFO_PATH "/tmp/"

typedef struct {
    char name[MAX_NAME_LENGTH];
    int score;
    pthread_t thread;
    FILE *fifo_file;
    pthread_mutex_t mutex;
} Player;

Player players[100]; // Suponemos un máximo de 100 jugadores
int num_players = 0;

void *fifo_reader(void *arg) {
    Player *player = (Player *)arg;
    char buffer[100];

    while (fgets(buffer, sizeof(buffer), player->fifo_file) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0'; // Eliminar el salto de línea del final
        char *endptr;
        int score = strtol(buffer, &endptr, 10); // Convertir la cadena a entero
        if (endptr != buffer && *endptr == '\0' && score >= 0) { // Si la conversión fue exitosa y el número es no negativo
            pthread_mutex_lock(&player->mutex); // Bloquear el mutex para evitar condiciones de carrera
            if (score > player->score) {
                player->score = score; // Actualizar la puntuación si es mayor que la puntuación actual
            }
            pthread_mutex_unlock(&player->mutex); // Desbloquear el mutex
        } else {
            // Error en la lectura del FIFO, eliminamos al jugador
            fclose(player->fifo_file);
            pthread_mutex_destroy(&player->mutex);
            printf("Error: Lectura inválida en el FIFO de %s. Eliminando al jugador.\n", player->name);
            return NULL;
        }
    }

    // Cerrar el FIFO y destruir el mutex
    fclose(player->fifo_file);
    pthread_mutex_destroy(&player->mutex);

    return NULL;
}

void new_player(const char *name) {
    // Verificar si el nombre ya existe
    for (int i = 0; i < num_players; i++) {
        if (strcmp(players[i].name, name) == 0) {
            printf("Error: El nombre de jugador ya existe.\n");
            return;
        }
    }

    // Verificar si el nombre es válido
    for (const char *c = name; *c != '\0'; c++) {
        if (!((*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z') || (*c >= '0' && *c <= '9'))) {
            printf("Error: El nombre de jugador solo puede contener letras y números.\n");
            return;
        }
    }

    // Construir el path del FIFO
    char fifo_path[100];
    snprintf(fifo_path, sizeof(fifo_path), "%s%s", FIFO_PATH, name);

    // Crear el FIFO
    if (mkfifo(fifo_path, 0666) == -1) {
        perror("Error creando FIFO");
        return;
    }

    // Abrir el FIFO para lectura
    int fifo_fd = open(fifo_path, O_RDONLY | O_NONBLOCK);
    if (fifo_fd == -1) {
        perror("Error abriendo FIFO");
        return;
    }

    // Abrir el descriptor de archivo en modo de lectura
    FILE *fifo_file = fdopen(fifo_fd, "r");
    if (fifo_file == NULL) {
        perror("Error abriendo descriptor de archivo");
        close(fifo_fd);
        unlink(fifo_path); // Eliminar el FIFO
        return;
    }

    // Inicializar el jugador
    Player player;
    strcpy(player.name, name);
    player.score = 0;
    player.fifo_file = fifo_file;
    pthread_mutex_init(&player.mutex, NULL);

    // Crear el hilo para leer el FIFO
    if (pthread_create(&player.thread, NULL, fifo_reader, (void *)&player) != 0) {
        perror("Error creando hilo");
        fclose(fifo_file);
        unlink(fifo_path); // Eliminar el FIFO
        return;
    }

    // Añadir el jugador al arreglo de jugadores
    players[num_players++] = player;

    printf("Nuevo jugador creado: %s\n", name);
}

void del_player(const char *name) {
    int player_index = -1;

    // Buscar al jugador por nombre
    for (int i = 0; i < num_players; i++) {
        if (strcmp(players[i].name, name) == 0) {
            player_index = i;
            break;
        }
    }

    if (player_index == -1) {
        printf("Error: El jugador \"%s\" no existe.\n", name);
        return;
    }

    // Cancelar el hilo
    pthread_cancel(players[player_index].thread);

    // Esperar a que el hilo termine
    pthread_join(players[player_index].thread, NULL);

    // Cerrar el archivo FIFO
    fclose(players[player_index].fifo_file);

    // Destruir el mutex
    pthread_mutex_destroy(&players[player_index].mutex);

    // Eliminar al jugador del arreglo de jugadores
    for (int i = player_index; i < num_players - 1; i++) {
        players[i] = players[i + 1];
    }
    num_players--;

    printf("Jugador \"%s\" eliminado.\n", name);
}

void highscore() {
    printf("Puntuaciones máximas:\n");
    for (int i = 0; i < num_players; i++) {
        printf("%s:%d\n", players[i].name, players[i].score);
    }
}

void reset(const char *name) {
    if (name == NULL) {
        // Resetear todas las puntuaciones
        for (int i = 0; i < num_players; i++) {
            pthread_mutex_lock(&players[i].mutex); // Bloquear el mutex antes de modificar la puntuación
            players[i].score = 0;
            pthread_mutex_unlock(&players[i].mutex); // Desbloquear el mutex
        }
        printf("Todas las puntuaciones han sido reseteadas.\n");
    } else {
        // Resetear la puntuación de un jugador específico
        int player_index = -1;
        for (int i = 0; i < num_players; i++) {
            if (strcmp(players[i].name, name) == 0) {
                player_index = i;
                break;
            }
        }
        if (player_index == -1) {
            printf("Error: El jugador \"%s\" no existe.\n", name);
            return;
        }
        pthread_mutex_lock(&players[player_index].mutex); // Bloquear el mutex antes de modificar la puntuación
        players[player_index].score = 0;
        pthread_mutex_unlock(&players[player_index].mutex); // Desbloquear el mutex
        printf("Puntuación de \"%s\" reseteada a 0.\n", name);
    }
}

int main() {
    char input[100];
    char command[100];
    char arg[100];

    while (1) {
        printf("Ingrese un comando: ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        // Eliminar el salto de línea del final
        input[strcspn(input, "\n")] = '\0';

        // Leer el comando y el argumento (si lo hay)
        if (sscanf(input, "%s %s", command, arg) == 1) {
            arg[0] = '\0'; // No hay argumento
        }

        if (strcmp(command, "newplayer") == 0) {
            new_player(arg);
        } else if (strcmp(command, "delplayer") == 0) {
            del_player(arg);
        } else if (strcmp(command, "highscore") == 0) {
            highscore();
        } else if (strcmp(command, "reset") == 0) {
            reset(arg[0] != '\0' ? arg : NULL);
        } else {
            printf("Error: Comando no reconocido.\n");
        }
    }

    // Liberar todos los recursos al salir
    for (int i = 0; i < num_players; i++) {
        pthread_cancel(players[i].thread);
        pthread_join(players[i].thread, NULL);
        fclose(players[i].fifo_file);
        pthread_mutex_destroy(&players[i].mutex);
        char fifo_path[100];
        snprintf(fifo_path, sizeof(fifo_path), "%s%s", FIFO_PATH, players[i].name);
        unlink(fifo_path);
    }

    return 0;
}
