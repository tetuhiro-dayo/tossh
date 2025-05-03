#include "history.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "const.h"

static char *history_buf[MAX_HISTORY];
static int hist_count = 0;
static char hist_file_path[1024] = {0};

void init_history(const char *filepath) {
    const char *path = filepath;
    if (!path) {
        const char *home = getenv("HOME");
        if (!home) home = ".";
        snprintf(hist_file_path, sizeof(hist_file_path), "%s/.tossh_history", home);
        path = hist_file_path;
    } else {
        strncpy(hist_file_path, filepath, sizeof(hist_file_path)-1);
        hist_file_path[sizeof(hist_file_path)-1] = '\0';
    }
    FILE *fp = fopen(path, "r");
    if (!fp) return;
    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        if (hist_count < MAX_HISTORY) {
            history_buf[hist_count++] = strdup(line);
        } else {
            free(history_buf[0]);
            memmove(history_buf, history_buf+1, sizeof(char*)*(MAX_HISTORY-1));
            history_buf[MAX_HISTORY-1] = strdup(line);
        }
    }
    fclose(fp);
}

void add_history(const char *cmd) {
    if (!cmd || !*cmd) return;
    if (hist_count < MAX_HISTORY) {
        history_buf[hist_count++] = strdup(cmd);
    } else {
        free(history_buf[0]);
        memmove(history_buf, history_buf+1, sizeof(char*)*(MAX_HISTORY-1));
        history_buf[MAX_HISTORY-1] = strdup(cmd);
    }
}

const char* get_history(int idx) {
    if (idx < 0 || idx >= hist_count) return NULL;
    return history_buf[idx];
}

int history_count(void) {
    return hist_count;
}

void save_history(void) {
    FILE *fp = fopen(hist_file_path, "w");
    if (!fp) return;
    int start = hist_count > MAX_HISTORY ? hist_count - MAX_HISTORY : 0;
    for (int i = start; i < hist_count; i++) {
        fprintf(fp, "%s\n", history_buf[i]);
    }
    fclose(fp);
}
