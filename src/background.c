#include "background.h"
#include "const.h"
#include <stddef.h>

pid_t bg_pids[MAX_ARGS];
int bg_count = 0;

void waitForBackground() {
    for (int i = 0; i < bg_count; i++) {
        waitpid(bg_pids[i], NULL, 0);
    }
    bg_count = 0;
}
