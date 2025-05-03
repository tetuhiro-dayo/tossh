#include "completion.h"
#include "const.h"
#include <dirent.h>
#include <stddef.h>

// Simple completion: scan current dir and PATH
void completion(const char *buf, int len) {
    int printed = 0;
    // current directory
    DIR *cd = opendir(".");
    if (cd) {
        struct dirent *ent;
        while ((ent = readdir(cd))) {
            if (strncmp(ent->d_name, buf, len) == 0) {
                printf("%s  ", ent->d_name);
                printed = 1;
            }
        }
        closedir(cd);
    }
    // PATH
    char *path = getenv("PATH");
    char *dirs = strdup(path ? path : "");
    char *dir = strtok(dirs, ":");
    while (dir) {
        DIR *d = opendir(dir);
        if (d) {
            struct dirent *ent;
            while ((ent = readdir(d))) {
                if (strncmp(ent->d_name, buf, len) == 0) {
                    printf("%s  ", ent->d_name);
                    printed = 1;
                }
            }
            closedir(d);
        }
        dir = strtok(NULL, ":");
    }
    free(dirs);
    if (printed) printf("\n");
}