#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#define FIFO_PATH "/tmp/"

typedef struct {
    char name[50];
    int score;
    pthread_t thread;
    int fifo_fd;
    pthread_mutex_t mutex;
} Player;

Player players[50];
int num_players = 0;

void *player_thread(void *arg) {
    Player *player = (Player *)arg;
    char buffer[256];

    while (1) {
        char *fgets_result = fgets(buffer, sizeof(buffer), fdopen(player->fifo_fd, "r"));
        if (fgets_result == NULL) {
            fprintf(stderr, "Error reading from FIFO for player %s\n", player->name);
            break;
        }

        int new_score = atoi(buffer);

        if (new_score >= 0) {
            pthread_mutex_lock(&player->mutex);
            if (new_score > player->score) {
                player->score = new_score;
                printf("Updated score of player %s to %d\n", player->name, player->score);
            }
            pthread_mutex_unlock(&player->mutex);
        } else {
            fprintf(stderr, "Invalid score received for player %s, removing player\n", player->name);
            close(player->fifo_fd);
            pthread_cancel(player->thread);
            pthread_mutex_destroy(&player->mutex);
            unlink(player->name);
            memset(player, 0, sizeof(Player));
            break;
        }
    }

    return NULL;
}

void new_player(const char *name) {
    if (num_players >= 50) {
        fprintf(stderr, "Cannot add more players, limit reached\n");
        return;
    }

    for (int i = 0; i < num_players; ++i) {
        if (strcmp(players[i].name, name) == 0) {
            fprintf(stderr, "Player with name %s already exists\n", name);
            return;
        }
    }

    char fifo_path[100];
    sprintf(fifo_path, "%s%s", FIFO_PATH, name);

    if (mkfifo(fifo_path, 0666) == -1) {
        fprintf(stderr, "Error creating FIFO for player %s: %s\n", name, strerror(errno));
        return;
    }

    int fifo_fd = open(fifo_path, O_RDONLY | O_NONBLOCK);
    if (fifo_fd == -1) {
        fprintf(stderr, "Error opening FIFO for player %s: %s\n", name, strerror(errno));
        unlink(fifo_path);
        return;
    }

    Player *player = &players[num_players++];
    strcpy(player->name, name);
    player->score = 0;
    player->fifo_fd = fifo_fd;
    pthread_mutex_init(&player->mutex, NULL);
    pthread_create(&player->thread, NULL, player_thread, (void *)player);

    printf("New player %s added\n", name);
}

void del_player(const char *name) {
    for (int i = 0; i < num_players; ++i) {
        if (strcmp(players[i].name, name) == 0) {
            close(players[i].fifo_fd);
            pthread_cancel(players[i].thread);
            pthread_mutex_destroy(&players[i].mutex);
            unlink(players[i].name);
            memset(&players[i], 0, sizeof(Player));
            printf("Player %s removed\n", name);
            return;
        }
    }

    fprintf(stderr, "Player with name %s not found\n", name);
}

void highscore() {
    for (int i = 0; i < num_players; ++i) {
        pthread_mutex_lock(&players[i].mutex);
        printf("%s:%d\n", players[i].name, players[i].score);
        pthread_mutex_unlock(&players[i].mutex);
    }
}

void reset(const char *name) {
    if (name == NULL) {
        for (int i = 0; i < num_players; ++i) {
            pthread_mutex_lock(&players[i].mutex);
            players[i].score = 0;
            pthread_mutex_unlock(&players[i].mutex);
        }
        printf("All scores reset to 0\n");
    } else {
        for (int i = 0; i < num_players; ++i) {
            if (strcmp(players[i].name, name) == 0) {
                pthread_mutex_lock(&players[i].mutex);
                players[i].score = 0;
                pthread_mutex_unlock(&players[i].mutex);
                printf("Score of player %s reset to 0\n", name);
                return;
            }
        }
        fprintf(stderr, "Player with name %s not found\n", name);
    }
}

int main() {
    while (1) {
        printf(">");
        char command[100];
        fgets(command, sizeof(command), stdin);

        char *token = strtok(command, " \n");
        if (token == NULL) continue;

        if (strcmp(token, "newplayer") == 0) {
            token = strtok(NULL, " \n");
            if (token != NULL) {
                new_player(token);
            } else {
                fprintf(stderr, "Error: Missing player name\n");
            }
        } else if (strcmp(token, "delplayer") == 0) {
            token = strtok(NULL, " \n");
            if (token != NULL) {
                del_player(token);
            } else {
                fprintf(stderr, "Error: Missing player name\n");
            }
        } else if (strcmp(token, "highscore") == 0) {
            highscore();
        } else if (strcmp(token, "reset") == 0) {
            token = strtok(NULL, " \n");
            reset(token);
        } else {
            fprintf(stderr, "Error: Unknown command\n");
        }
    }

    return 0;
}
