#ifndef HISTORY_H
#define HISTORY_H

#define MAX_HISTORY 100

void init_history(const char *filepath);
void add_history(const char *cmd);
const char* get_history(int idx);
int history_count();
void save_history();
#endif