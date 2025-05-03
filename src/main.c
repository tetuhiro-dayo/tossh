#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>
#include <termios.h>

#include "executor.h"
#include "background.h"
#include "signal_handler.h"

#define MAX_CMD_LENGTH 1024
#define MAX_HISTORY 100

char history[MAX_HISTORY][MAX_CMD_LENGTH];
int history_count = 0;
static struct termios orig_termios;

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

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

int main() {
    signal(SIGINT, sigint_handler);
    enableRawMode();

    while (1) {
        printf("[ tossh ]$ "); fflush(stdout);
        char buf[MAX_CMD_LENGTH] = {0};
        int pos = 0;

        // read input
        while (1) {
            char c;
            if (read(STDIN_FILENO, &c, 1) <= 0) continue;
            if (c == '\n' || c == '\r') {
                printf("\n");
                break;
            } else if (c == 127 || c == 8) {
                if (pos > 0) {
                    pos--; buf[pos] = '\0';
                    printf("\b \b"); fflush(stdout);
                }
            } else if (c == '\t') {
                completion(buf, pos);
                printf("[ tossh ]$ %s", buf);
                fflush(stdout);
            } else {
                if (pos < MAX_CMD_LENGTH - 1) {
                    buf[pos++] = c;
                    buf[pos] = '\0';
                    printf("%c", c);
                    fflush(stdout);
                }
            }
        }
        if (pos == 0) continue;
        strncpy(history[history_count % MAX_HISTORY], buf, MAX_CMD_LENGTH);
        history_count++;

        // prepare command
        char command[MAX_CMD_LENGTH];
        strncpy(command, buf, MAX_CMD_LENGTH);

        // built-in
        if (strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0 || strcmp(command, "bye") == 0) break;
        if (strcmp(command, "wait") == 0) { waitForBackground(); continue; }
        if (command[0] == '!' && isdigit((unsigned char)command[1])) {
            int num = atoi(&command[1]);
            if (num > 0 && num <= history_count) {
                char *replay = history[(num - 1) % MAX_HISTORY];
                printf("Replaying: %s\n", replay);
                exec(replay);
            } else {
                printf("No such command in history!\n");
            }
            continue;
        }
        if (strncmp(command, "run ", 4) == 0 || strncmp(command, "runscript ", 10) == 0) {
            char *raw = command + (strncmp(command, "run ", 4) == 0 ? 4 : 10);
            // trim trailing dots
            size_t rlen = strlen(raw);
            while (rlen && raw[rlen - 1] == '.') raw[--rlen] = '\0';
            char filename[MAX_CMD_LENGTH];
            char *dot = strrchr(raw, '.');
            if (!dot || strcmp(dot, ".tossh") != 0)
                snprintf(filename, sizeof(filename), "%s.tossh", raw);
            else
                strncpy(filename, raw, sizeof(filename));
            FILE *fp = fopen(filename, "r");
            if (!fp) { perror("fopen"); continue; }
            char line[MAX_CMD_LENGTH];
            while (fgets(line, sizeof(line), fp)) {
                line[strcspn(line, "\n")] = '\0';
                if (strlen(line)) exec(line);
            }
            fclose(fp);
            continue;
        }
        if (strcmp(command, "hello") == 0) { printf("Hello, World!\n"); continue; }
        if (strcmp(command, "ver") == 0) { printf("tossh version 1.2.0\n"); continue; }
        if (strcmp(command, "fortune") == 0) {
            const char *f[] = {"Great luck...","Good luck...","Small luck...","Little luck...","Bad luck..."};
            printf("%s\n", f[rand() % 5]); continue;
        }
        if (strcmp(command, "history") == 0) {
            int start = history_count > MAX_HISTORY ? history_count - MAX_HISTORY : 0;
            for (int i = start; i < history_count; i++)
                printf("%d: %s\n", i + 1, history[i % MAX_HISTORY]);
            continue;
        }
        // external
        exec(command);
    }
    disableRawMode();
    return 0;
}
