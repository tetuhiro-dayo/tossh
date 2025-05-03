#include "alias.h"

Alias aliases[MAX_ALIAS];
int alias_count = 0;

void expandAlias(char *command) {
    for (int i = 0; i < alias_count; i++) {
        if (strcmp(command, aliases[i].name) == 0) {
            strncpy(command, aliases[i].command, MAX_CMD_LENGTH);
            break;
        }
    }
}

void registerAlias(const char *alias_definition) {
    char temp[MAX_CMD_LENGTH];
    strncpy(temp, alias_definition, MAX_CMD_LENGTH);
    char *equal_sign = strchr(temp, '=');
    if (equal_sign != NULL) {
        *equal_sign = '\0';
        char *name = temp;
        char *cmd = equal_sign + 1;
        if (alias_count < MAX_ALIAS) {
            strncpy(aliases[alias_count].name, name, MAX_CMD_LENGTH);
            strncpy(aliases[alias_count].command, cmd, MAX_CMD_LENGTH);
            alias_count++;
            printf("Alias registered: %s -> %s\n", name, cmd);
        } else {
            fprintf(stderr, "Alias limit reached!\n");
        }
    } else {
        fprintf(stderr, "Invalid alias format. Use alias name=command\n");
    }
}
