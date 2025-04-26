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

pid_t bg_pids[MAX_ARGS];
int bg_count = 0;
char history[MAX_HISTORY][MAX_CMD_LENGTH];
int history_count = 0;

int isCommand(const char *input, const char *command) {
    return strcmp(input, command) == 0;
}

void executeCommandWithMultiplePipes(char *command) {
    char *cmds[MAX_ARGS];
    int cmd_count = 0;
    int background = 0;

    // バックグラウンド実行チェック
    char *bg_check = strchr(command, '&');
    if (bg_check != NULL) {
        background = 1;
        *bg_check = '\0';
    }

    char *token = strtok(command, "|");
    while (token != NULL && cmd_count < MAX_ARGS - 1) {
        cmds[cmd_count++] = token;
        token = strtok(NULL, "|");
    }
    cmds[cmd_count] = NULL;

    int pipefd[2 * (cmd_count - 1)];
    for (int i = 0; i < cmd_count - 1; i++) {
        if (pipe(pipefd + i*2) < 0) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < cmd_count; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            if (i != 0) {
                dup2(pipefd[(i-1)*2], STDIN_FILENO);
            }
            if (i != cmd_count - 1) {
                dup2(pipefd[i*2 + 1], STDOUT_FILENO);
            }

            for (int j = 0; j < 2 * (cmd_count - 1); j++) {
                close(pipefd[j]);
            }

            char *args[MAX_ARGS];
            int k = 0;
            char *arg = strtok(cmds[i], " ");
            while (arg != NULL && k < MAX_ARGS - 1) {
                args[k++] = arg;
                arg = strtok(NULL, " ");
            }
            args[k] = NULL;

            if (i == cmd_count - 1) {
                for (int m = 0; args[m] != NULL; m++) {
                    if (strcmp(args[m], ">") == 0 || strcmp(args[m], ">>") == 0) {
                        int append = (strcmp(args[m], ">>") == 0);
                        char *filename = args[m + 1];
                        int fd = open(filename, O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC), 0644);
                        if (fd < 0) {
                            perror("open");
                            exit(EXIT_FAILURE);
                        }
                        dup2(fd, STDOUT_FILENO);
                        close(fd);
                        args[m] = NULL;
                        break;
                    }
                }
            }

            execvp(args[0], args);
            perror("execvp");
            exit(EXIT_FAILURE);
        } else if (background) {
            bg_pids[bg_count++] = pid;
        }
    }

    for (int i = 0; i < 2 * (cmd_count - 1); i++) {
        close(pipefd[i]);
    }

    if (!background) {
        for (int i = 0; i < cmd_count; i++) {
            wait(NULL);
        }
    }
}

void waitForBackground() {
    for (int i = 0; i < bg_count; i++) {
        waitpid(bg_pids[i], NULL, 0);
    }
    bg_count = 0;
}

// ここから main 関数
int main() {
    char command[MAX_CMD_LENGTH];

    while (1) {
        printf(COLOR_CYAN "[ tossh ]$ " COLOR_RESET);
        fflush(stdout);

        if (fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }

        command[strcspn(command, "\n")] = 0;

        if (strlen(command) > 0) {
            strncpy(history[history_count % MAX_HISTORY], command, MAX_CMD_LENGTH);
            history_count++;
        }

        if (isCommand(command, "exit") || isCommand(command, "quit") || isCommand(command, "bye")) {
            break;
        }

        if (isCommand(command, "wait")) {
            waitForBackground();
            continue;
        }

        if (isCommand(command, "hello")) {
            printf("Hello, World!\n");
            continue;
        }

        if (isCommand(command, "ver")) {
            printf("tossh version 1.2.0\n");
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

        executeCommandWithMultiplePipes(command);
    }

    return 0;
}
