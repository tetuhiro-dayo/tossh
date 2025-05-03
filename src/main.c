#include "const.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "terminal.h"
#include "input.h"
#include "completion.h"
#include "history.h"
#include "builtin.h"
#include "executor.h"
#include "background.h"
#include "signal_handler.h"

int main(void) {
    // シグナルハンドラ登録
    signal(SIGINT, sigint_handler);
    signal(SIGCHLD, sigchld_handler);

    // 端末を Raw モードに設定
    enableRawMode();

    // 履歴をロード（NULL でデフォルト ~/.tossh_history）
    init_history(NULL);

    while (1) {
        // プロンプト表示
        printf(COLOR_CYAN PROMPT COLOR_RESET);
        fflush(stdout);

        // 入力行を取得（EOF は負値を返す）
        char buf[MAX_CMD_LENGTH];
        int len = readLine(buf, sizeof(buf));
        if (len < 0) {
            // Ctrl-D などで終了
            break;
        }

        // 履歴に追加
        add_history(buf);

        // コマンド分解
       char *argv[MAX_ARGS];
        int argc = tokenize(buf, argv);

        // ビルトインかどうか処理
        if (argc > 0 && handle_builtin(buf,argc, argv)) {
            continue;
        }

        // 外部コマンド／バックグラウンド実行
        exec(buf);
    }

    // 履歴を書き出し
    save_history();

    // Raw モード解除
    disableRawMode();
    return 0;
}
