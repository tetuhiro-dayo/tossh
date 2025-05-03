#include "signal_handler.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "const.h"

pid_t current_pid = -1;
int bg_count = 0;
pid_t bg_pids[100];

void sigint_handler(int signo) {
    if (current_pid > 0) {
        kill(current_pid, SIGINT);
    }
    printf("\n");
}

void sigchld_handler(int signo) {
    (void)signo;
    // 終了した子プロセスを回収
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        // バックグラウンドプロセスリストから削除
        for (int i = 0; i < bg_count; i++) {
            if (bg_pids[i] == pid) {
                // リストから詰める
                for (int j = i; j < bg_count - 1; j++) {
                    bg_pids[j] = bg_pids[j + 1];
                }
                bg_count--;
                break;
            }
        }
    }
}
