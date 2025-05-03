#include "terminal.h"
#include <unistd.h>

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