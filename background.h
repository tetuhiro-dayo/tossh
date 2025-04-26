#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <sys/types.h>
#include <sys/wait.h>

#define MAX_ARGS 64

extern pid_t bg_pids[MAX_ARGS];
extern int bg_count;

void waitForBackground(void);

#endif
