#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#define MAX_LINE 256

struct Sourcefiles {
    char *path;
    long cfiles;
    long hfiles;
    long long totalbytes;
};

typedef struct Sourcefiles Sourcefiles;

void usage(void) {
    fprintf(stderr, "usage: ./sourcefiles <directory>\n");
    exit(EXIT_FAILURE);
}

void recursive(char *path) {
    DIR *dir;
    struct dirent *ent;
    struct stat sb;

    dir = opendir(path);
    if (dir == NULL) {
        err(EXIT_FAILURE, "opendir failed: %s", path);
    }

    char fullpath[MAX_LINE];
    size_t path_len = strlen(path);
    if (path_len >= MAX_LINE - 1) {
        errx(EXIT_FAILURE, "Path too long");
    }
    strncpy(fullpath, path, MAX_LINE);
    fullpath[path_len] = '/'; // Agregar barra inclinada al final del path
    fullpath[MAX_LINE - 1] = '\0'; // Asegurarse de que el búfer esté terminado con nulo

    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_name[0] != '.') {
            size_t name_len = strlen(ent->d_name);
            if (path_len + name_len + 2 > MAX_LINE) {
                errx(EXIT_FAILURE, "Path too long");
            }
            strncpy(fullpath + path_len + 1, ent->d_name, MAX_LINE - path_len - 1);
            fullpath[MAX_LINE - 1] = '\0'; // Asegurarse de que el búfer esté terminado con nulo

            if (lstat(fullpath, &sb) < 0) {
                err(EXIT_FAILURE, "lstat");
            }

            if ((sb.st_mode & S_IFMT) == S_IFDIR) {
                recursive(fullpath);
            } else if ((sb.st_mode & S_IFMT) == S_IFREG) {
                printf("%s\n", fullpath);
            }
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        usage();
    }

    recursive(argv[1]);

    exit(EXIT_SUCCESS);
}
