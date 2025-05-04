#include "executor.h"
#include "background.h"
#include "const.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <ctype.h>

/**
 * トークナイズ関数: 空白とクォートに対応
 * @param s: 入力文字列 (破壊的書き換えされる)
 * @param argv: トークンを格納する配列
 * @return トークン数
 */
int tokenize(char *s, char **argv) {
    int argc = 0;
    char *p = s;
    while (*p && argc < MAX_ARGS) {
        while (*p && isspace((unsigned char)*p)) p++;
        if (!*p) break;
        if (*p == '"' || *p == '\'') {
            char quote = *p++;
            argv[argc++] = p;
            while (*p && *p != quote) p++;
            if (*p == quote) *p++ = '\0';
        } else {
            argv[argc++] = p;
            while (*p && !isspace((unsigned char)*p)) p++;
            if (*p) *p++ = '\0';
        }
    }
    argv[argc] = NULL;
    return argc;
}

void exec(char *command) {
    char *cmds[MAX_ARGS];
    int cmd_count = 0;
    int background = 0;

    // バックグラウンド実行チェック
    char *bg_check = strchr(command, '&');
    if (bg_check) {
        background = 1;
        *bg_check = '\0';
    }

    // パイプで分割
    char *tok = strtok(command, "|");
    while (tok && cmd_count < MAX_ARGS - 1) {
        cmds[cmd_count++] = tok;
        tok = strtok(NULL, "|");
    }
    cmds[cmd_count] = NULL;

    // パイプ生成
    int pipefd[2 * (cmd_count - 1)];
    for (int i = 0; i < cmd_count - 1; i++) {
        if (pipe(pipefd + i*2) < 0) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < cmd_count; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) {
            // 子プロセス: 入出力リダイレクト
            if (i > 0) dup2(pipefd[(i-1)*2], STDIN_FILENO);
            if (i < cmd_count - 1) dup2(pipefd[i*2 + 1], STDOUT_FILENO);
            for (int j = 0; j < 2*(cmd_count - 1); j++) close(pipefd[j]);

            // トークナイズ
            char *args[MAX_ARGS];
            int argc = tokenize(cmds[i], args);

            // 最終コマンドで I/O リダイレクト
            if (i == cmd_count - 1) {
                for (int j = 0; j < argc; j++) {
                    if (strcmp(args[j], ">") == 0 || strcmp(args[j], ">>") == 0) {
                        int append = (args[j][1] == '>');
                        char *filename = args[j+1];
                        int fd = open(filename, O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC), 0644);
                        if (fd < 0) { perror("open"); exit(EXIT_FAILURE); }
                        dup2(fd, STDOUT_FILENO);
                        close(fd);
                        args[j] = NULL;
                        break;
                    }
                }
            }

            // シグナルハンドラをデフォルトに戻す
            signal(SIGINT, SIG_DFL);
            execvp(args[0], args);
            perror("execvp");
            exit(EXIT_FAILURE);
        } else {
            // 親プロセス: ジョブテーブル登録
            if (!background) current_pid = pid;
            else bg_pids[bg_count++] = pid;
        }
    }
    for (int i = 0; i < 2*(cmd_count - 1); i++) close(pipefd[i]);

    // フォアグラウンドなら待機
    if (!background) {
        for (int i = 0; i < cmd_count; i++) wait(NULL);
        current_pid = -1;
    }
}
