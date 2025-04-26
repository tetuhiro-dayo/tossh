#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <signal.h>

extern pid_t current_pid;

void sigint_handler(int signo);

#endif
