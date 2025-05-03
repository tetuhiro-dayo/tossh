#include "input.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "completion.h"
#include "history.h"
#include "const.h"

int readLine(char *buf, size_t maxlen) {
    size_t pos = 0;
    while (1) {
        char c;
        ssize_t n = read(STDIN_FILENO, &c, 1);
        if (n <= 0) {
            // EOF またはエラー
            return -1;
        }
        if (c == '\n' || c == '\r') {
            buf[pos] = '\0';
            putchar('\n');
            return (int)pos;
        }
        // バックスペース
        else if (c == 127 || c == 8) {
            if (pos > 0) {
                pos--;
                buf[pos] = '\0';
                // 画面上から一文字消す
                printf("\b \b");
                fflush(stdout);
            }
        }
        // タブで補完
        else if (c == '\t') {
            completion(buf, pos);
            // プロンプトとバッファを再描画
            printf("\r"PROMPT"%s", buf);
            fflush(stdout);
        }
        // 矢印キーなど（ANSI エスケープシーケンス）
        else if (c == ESC) {
            char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) == 1 &&
                read(STDIN_FILENO, &seq[1], 1) == 1) {
                if (seq[0] == '[') {
                    switch (seq[1]) {
                        case 'A': // 上矢印
                            // TODO: 履歴から前のコマンドを取得
                            break;
                        case 'B': // 下矢印
                            // TODO: 履歴から次のコマンドを取得
                            break;
                        // TODO: 左右キー ('C','D') も処理
                    }
                }
            }
        }
        // 通常の印字可能文字
        else if (isprint((unsigned char)c)) {
            if (pos < maxlen - 1) {
                buf[pos++] = c;
                buf[pos] = '\0';
                putchar(c);
                fflush(stdout);
            }
        }
    }
}
