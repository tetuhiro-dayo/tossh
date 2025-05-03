#ifndef TERMINAL_H
#define TERMINAL_H

#include <termios.h>

void disableCanonicalMode(struct termios *original_termios);
void restoreCanonicalMode(struct termios *original_termios);
void enableRawMode();
void disableRawMode();

#endif