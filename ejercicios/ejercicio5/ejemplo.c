#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define MAX_PATH_LENGTH 1024

int is_regular_file(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

void count_files(const char *path, int *c_count, int *h_count, long *total_size) {
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(path))) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            char new_path[MAX_PATH_LENGTH];
            snprintf(new_path, sizeof(new_path), "%s/%s", path, entry->d_name);
            count_files(new_path, c_count, h_count, total_size);
        } else if (entry->d_type == DT_REG) {
            if (strstr(entry->d_name, ".c")) {
                (*c_count)++;
            } else if (strstr(entry->d_name, ".h")) {
                (*h_count)++;
            }
            char file_path[MAX_PATH_LENGTH];
            snprintf(file_path, sizeof(file_path), "%s/%s", path, entry->d_name);
            if (is_regular_file(file_path)) {
                struct stat st;
                if (stat(file_path, &st) == 0) {
                    *total_size += st.st_size;
                }
            }
        }
    }
    closedir(dir);
}

int main() {
    char path[MAX_PATH_LENGTH];
    while (fgets(path, sizeof(path), stdin) != NULL) {
        // Eliminar el salto de línea del final
        path[strcspn(path, "\n")] = 0;

        int c_count = 0, h_count = 0;
        long total_size = 0;
        count_files(path, &c_count, &h_count, &total_size);
        printf("%s\t%d\t%d\t%ld\n", path, c_count, h_count, total_size);
    }
    return 0;
}
