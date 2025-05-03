#include "terminal.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include "const.h"

static struct termios orig_termios;

void disableCanonicalMode(struct termios *original_termios) {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, original_termios);
    new_termios = *original_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

void restoreCanonicalMode(struct termios *original_termios) {
    tcsetattr(STDIN_FILENO, TCSANOW, original_termios);
}

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}