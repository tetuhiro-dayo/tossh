#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <sys/types.h>

#define MAX_ARGS 64

extern pid_t current_pid;
extern pid_t bg_pids[MAX_ARGS];
extern int bg_count;

void exec(char *command);

#endif
