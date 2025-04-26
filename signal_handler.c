#include "signal_handler.h"
#include <stdio.h>
#include <unistd.h>

pid_t current_pid = -1;

void sigint_handler(int signo) {
    if (current_pid > 0) {
        kill(current_pid, SIGINT);
    }
    printf("\n");
}