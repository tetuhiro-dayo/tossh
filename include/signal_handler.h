#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <signal.h>
#include <sys/types.h>

extern pid_t current_pid;
extern int bg_count;
extern pid_t bg_pids[100];  // 適切なサイズを設定

/**
 * SIGINT ハンドラ: フォアグラウンドプロセスに SIGINT を転送
 */
void sigint_handler(int signo);

/**
 * SIGCHLD ハンドラ: 子プロセス（バックグラウンド含む）の終了を回収
 */
void sigchld_handler(int signo);

#endif
