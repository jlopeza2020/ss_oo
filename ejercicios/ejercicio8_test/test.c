#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MAX_PLAYERS 100
#define FIFO_PATH "/tmp/"

typedef struct {
    char name[50];
    int score;
    char fifo_name[100];
    pthread_t thread;
    int fifo_fd;
} Player;

Player players[MAX_PLAYERS];
int num_players = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *fifo_thread(void *arg) {
    Player *player = (Player *)arg;
    char fifo_path[200]; // Aumentar el tamaño del búfer

    snprintf(fifo_path, sizeof(fifo_path), "%s%s", FIFO_PATH, player->fifo_name);

    mkfifo(fifo_path, 0666);
    player->fifo_fd = open(fifo_path, O_RDONLY);
    if (player->fifo_fd == -1) {
        perror("Error opening fifo");
        exit(EXIT_FAILURE);
    }

    while (1) {
        char buf[100];
        ssize_t bytes_read = read(player->fifo_fd, buf, sizeof(buf) - 1);
        if (bytes_read > 0) {
            buf[bytes_read] = '\0';
            int new_score = atoi(buf);
            if (new_score >= 0) {
                pthread_mutex_lock(&mutex);
                if (new_score > player->score) {
                    player->score = new_score;
                }
                pthread_mutex_unlock(&mutex);
            } else {
                pthread_mutex_lock(&mutex);
                close(player->fifo_fd);
                unlink(fifo_path);
                player->score = 0;
                pthread_mutex_unlock(&mutex);
                break;
            }
        }
    }

    return NULL;
}

void add_player(char *name) {
    pthread_mutex_lock(&mutex);
    if (num_players < MAX_PLAYERS) {
        strcpy(players[num_players].name, name);
        sprintf(players[num_players].fifo_name, "%s_fifo", name);
        players[num_players].score = 0;
        pthread_create(&players[num_players].thread, NULL, fifo_thread, (void *)&players[num_players]);
        num_players++;
        printf("Player '%s' added.\n", name);
    } else {
        printf("Maximum number of players reached.\n");
    }
    pthread_mutex_unlock(&mutex);
}

void del_player(char *name) {
    pthread_mutex_lock(&mutex);
    int i;
    for (i = 0; i < num_players; i++) {
        if (strcmp(players[i].name, name) == 0) {
            pthread_cancel(players[i].thread);
            pthread_join(players[i].thread, NULL);
            close(players[i].fifo_fd);
            char fifo_path[200]; // Aumentar el tamaño del búfer

            snprintf(fifo_path, sizeof(fifo_path), "%s%s", FIFO_PATH, players[i].fifo_name);
            unlink(fifo_path);
            memmove(&players[i], &players[i + 1], (num_players - i - 1) * sizeof(Player));
            num_players--;
            printf("Player '%s' deleted.\n", name);
            break;
        }
    }
    if (i == num_players) {
        printf("Player '%s' not found.\n", name);
    }
    pthread_mutex_unlock(&mutex);
}

void highscore() {
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < num_players; i++) {
        printf("%s:%d\n", players[i].name, players[i].score);
    }
    pthread_mutex_unlock(&mutex);
}

void reset(char *name) {
    pthread_mutex_lock(&mutex);
    if (name == NULL) {
        for (int i = 0; i < num_players; i++) {
            players[i].score = 0;
        }
        printf("Scores reset for all players.\n");
    } else {
        int i;
        for (i = 0; i < num_players; i++) {
            if (strcmp(players[i].name, name) == 0) {
                players[i].score = 0;
                printf("Score reset for player '%s'.\n", name);
                break;
            }
        }
        if (i == num_players) {
            printf("Player '%s' not found.\n", name);
        }
    }
    pthread_mutex_unlock(&mutex);
}

int main() {
    while (1) {
        char command[100], arg[100];
        printf("Enter command: ");
        if (scanf("%99s", command) != 1) {
            break;
        }
        if (strcmp(command, "newplayer") == 0) {
            scanf("%99s", arg);
            add_player(arg);
        } else if (strcmp(command, "delplayer") == 0) {
            scanf("%99s", arg);
            del_player(arg);
        } else if (strcmp(command, "highscore") == 0) {
            highscore();
        } else if (strcmp(command, "reset") == 0) {
            scanf("%99s", arg);
            reset(arg[0] ? arg : NULL);
        } else {
            printf("Unknown command: %s\n", command);
        }
    }

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < num_players; i++) {
        pthread_cancel(players[i].thread);
        pthread_join(players[i].thread, NULL);
        close(players[i].fifo_fd);
        char fifo_path[200]; // Aumentar el tamaño del búfer

        snprintf(fifo_path, sizeof(fifo_path), "%s%s", FIFO_PATH, players[i].fifo_name);

        unlink(fifo_path);
    }
    pthread_mutex_unlock(&mutex);

    return 0;
}
