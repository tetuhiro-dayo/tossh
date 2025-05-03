#include "executor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>
#include <ctype.h>
#include <signal.h>
#include "alias.h"
#include "terminal.h"
#include "const.h"

void exec(char *command) {
    char *cmds[MAX_ARGS];
    int cmd_count = 0;
    int background = 0;

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
        } else {
            if (!background) {
                current_pid = pid;
            } else {
                bg_pids[bg_count++] = pid;
            }
        }
    }

    for (int i = 0; i < 2 * (cmd_count - 1); i++) {
        close(pipefd[i]);
    }

    if (!background) {
        for (int i = 0; i < cmd_count; i++) {
            wait(NULL);
        }
        current_pid = -1;
    }
}