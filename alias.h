#ifndef ALIAS_H
#define ALIAS_H

#include <stdio.h>
#include <string.h>

#define MAX_CMD_LENGTH 1024
#define MAX_ALIAS 32

typedef struct {
    char name[MAX_CMD_LENGTH];
    char command[MAX_CMD_LENGTH];
} Alias;

extern Alias aliases[MAX_ALIAS];
extern int alias_count;

void expandAlias(char *command);
void registerAlias(const char *alias_definition);

#endif