#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>
#include "alias.h"
#include "terminal.h"

#define MAX_CMD_LENGTH 1024
#define MAX_PATH_LENGTH 1024
#define MAX_ARGS 64
#define MAX_HISTORY 100

#define COLOR_RESET "\033[0m"
#define COLOR_CYAN "\033[36m"
#define COLOR_GREEN "\033[32m"
#define COLOR_RED "\033[31m"

int isCommand(const char *input, const char *command) {
    return strcmp(input, command) == 0;
}

int main() {
    char command[MAX_CMD_LENGTH] = {0};
    char cwd[MAX_PATH_LENGTH];
    char *args[MAX_ARGS];
    char history[MAX_HISTORY][MAX_CMD_LENGTH];
    int history_count = 0;
    struct termios original_termios;

    disableCanonicalMode(&original_termios);
    srand(time(NULL));

    while (1) {
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            fprintf(stderr, COLOR_RED);
            perror("getcwd");
            fprintf(stderr, COLOR_RESET);
            continue;
        }

        printf(COLOR_CYAN "[ tossh : %s ]$ " COLOR_RESET, cwd);
        fflush(stdout);

        int pos = 0;
        memset(command, 0, sizeof(command));

        while (1) {
            char c;
            if (read(STDIN_FILENO, &c, 1) <= 0) {
                continue;
            }

            if (c == '\n') {
                printf("\n");
                break;
            } else if (c == 127 || c == 8) {
                if (pos > 0) {
                    pos--;
                    command[pos] = '\0';
                    printf("\b \b");
                    fflush(stdout);
                }
            } else {
                if (pos < MAX_CMD_LENGTH - 1) {
                    command[pos++] = c;
                    printf("%c", c);
                    fflush(stdout);
                }
            }
        }

        if (strlen(command) > 0) {
            strncpy(history[history_count % MAX_HISTORY], command, MAX_CMD_LENGTH);
            history_count++;
        }

        if (strncmp(command, "alias ", 6) == 0) {
            registerAlias(command + 6);
            continue;
        }

        expandAlias(command);

        if (isCommand(command, "exit") || isCommand(command, "quit") || isCommand(command, "bye")) {
            printf(COLOR_GREEN "Good Bye." COLOR_RESET "\n");
            break;
        }

        if (strncmp(command, "cd ", 3) == 0) {
            char *path = command + 3;
            if (chdir(path) != 0) {
                fprintf(stderr, COLOR_RED);
                perror("cd");
                fprintf(stderr, COLOR_RESET);
            }
            continue;
        }

        if (isCommand(command, "hello")) {
            printf("Hello, World!\n");
            continue;
        }

        if (isCommand(command, "ver")) {
            printf("tossh version 1.0.0\n");
            continue;
        }

        if (isCommand(command, "time")) {
            system("date");
            continue;
        }

        if (isCommand(command, "joke")) {
            printf("Why do programmers prefer dark mode? Because light attracts bugs!\n");
            continue;
        }

        if (isCommand(command, "about")) {
            printf("tossh - Terminal Operating System Shell. Created by you!\n");
            continue;
        }

        if (isCommand(command, "fortune")) {
            const char *fortunes[] = {
                "Great luck: Today is your lucky day!",
                "Good luck: Something good will happen!",
                "Small luck: A small blessing awaits.",
                "Little luck: An okay day.",
                "Bad luck: Be careful today."
            };
            int n = sizeof(fortunes) / sizeof(fortunes[0]);
            printf("%s\n", fortunes[rand() % n]);
            continue;
        }

        if (isCommand(command, "history")) {
            int start = history_count > MAX_HISTORY ? history_count - MAX_HISTORY : 0;
            for (int i = start; i < history_count; i++) {
                printf("%d: %s\n", i + 1, history[i % MAX_HISTORY]);
            }
            continue;
        }

        int i = 0;
        char *token = strtok(command, " ");
        char *outfile = NULL;
        char *infile = NULL;
        int append_mode = 0;
        while (token != NULL && i < MAX_ARGS - 1) {
            if (strcmp(token, ">") == 0 || strcmp(token, ">>") == 0) {
                append_mode = (strcmp(token, ">>") == 0);
                token = strtok(NULL, " ");
                if (token != NULL) {
                    outfile = token;
                }
                break;
            } else if (strcmp(token, "<") == 0) {
                token = strtok(NULL, " ");
                if (token != NULL) {
                    infile = token;
                }
                break;
            }
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        pid_t pid = fork();
        if (pid == 0) {
            if (outfile) {
                int flags = O_WRONLY | O_CREAT | (append_mode ? O_APPEND : O_TRUNC);
                int fd = open(outfile, flags, 0644);
                if (fd < 0) {
                    fprintf(stderr, COLOR_RED);
                    perror("open outfile");
                    fprintf(stderr, COLOR_RESET);
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            if (infile) {
                int fd = open(infile, O_RDONLY);
                if (fd < 0) {
                    fprintf(stderr, COLOR_RED);
                    perror("open infile");
                    fprintf(stderr, COLOR_RESET);
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
            }
            execvp(args[0], args);
            fprintf(stderr, COLOR_RED);
            perror("execvp");
            fprintf(stderr, COLOR_RESET);
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            wait(NULL);
        } else {
            fprintf(stderr, COLOR_RED);
            perror("fork");
            fprintf(stderr, COLOR_RESET);
        }
    }

    restoreCanonicalMode(&original_termios);
    return 0;
}
