#include "builtin.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "history.h"
#include "executor.h"
#include "background.h"
#include "const.h"

bool handle_builtin(char *cmd) {
    // exit 系ビルトイン
    if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "quit") == 0 || strcmp(cmd, "bye") == 0) {
        printf("Bye.");
        exit(0);
    }
    // history の表示
    if (strcmp(cmd, "history") == 0) {
        int count = history_count();
        int start = count > MAX_HISTORY ? count - MAX_HISTORY : 0;
        for (int i = start; i < count; i++) {
            printf("%d: %s\n", i + 1, get_history(i));
        }
        return true;
    }
    // wait (バックグラウンドジョブの待機)
    if (strcmp(cmd, "wait") == 0) {
        waitForBackground();
        return true;
    }
    // !N: 履歴再実行
    if (cmd[0] == '!' && isdigit((unsigned char)cmd[1])) {
        int num = atoi(&cmd[1]);
        const char *replay = get_history(num - 1);
        if (replay) {
            printf("Replaying: %s\n", replay);
            exec((char *)replay);
        } else {
            printf("No such command in history!\n");
        }
        return true;
    }
    // hello, ver, fortune などのシンプルビルトイン
    if (strcmp(cmd, "hello") == 0) {
        printf("Hello, World!\n");
        return true;
    }
    if (strcmp(cmd, "ver") == 0) {
        printf("tossh version 1.2.0\n");
        return true;
    }
    if (strcmp(cmd, "fortune") == 0) {
        const char *f[] = {"Great luck","Good luck","Small luck","Little luck","Bad luck"};
        printf("%s\n", f[rand() % 5]);
        return true;
    }
    // run / runscript ビルトイン
    if (strncmp(cmd, "run ", 4) == 0 || strncmp(cmd, "runscript ", 10) == 0) {
        char *raw = cmd + (strncmp(cmd, "run ", 4) == 0 ? 4 : 10);
        // トレイリングドットをトリム
        size_t rlen = strlen(raw);
        while (rlen > 0 && raw[rlen - 1] == '.') raw[--rlen] = '\0';
        char filename[MAX_CMD_LENGTH];
        char *dot = strrchr(raw, '.');
        if (!dot || strcmp(dot, ".tossh") != 0) {
            snprintf(filename, sizeof(filename), "%s.tossh", raw);
        } else {
            strncpy(filename, raw, sizeof(filename));
        }
        FILE *fp = fopen(filename, "r");
        if (!fp) { perror("fopen"); return true; }
        char line[MAX_CMD_LENGTH];
        while (fgets(line, sizeof(line), fp)) {
            line[strcspn(line, "\n")] = '\0';
            if (strlen(line)) exec(line);
        }
        fclose(fp);
        return true;
    }

    // ビルトインコマンドに該当しなかった
    return false;
}
