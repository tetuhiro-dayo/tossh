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
#include "const.h"


int main() {
    // シグナルハンドラ登録
    signal(SIGINT, sigint_handler);

    // 端末を Raw モードに設定
    enableRawMode();

    // 履歴をロード（NULL でデフォルト ~/.tossh_history）
    init_history(NULL);

    while (1) {
        // プロンプト表示
        printf(PROMPT);
        fflush(stdout);

        // 入力行を取得（EOF は負値を返す）
        char buf[MAX_CMD_LENGTH] = {0};
        int len = readLine(buf, MAX_CMD_LENGTH);
        if (len <= 0) {
            // Ctrl-D などで終了
            break;
        }

        // 履歴に追加
        add_history(buf);

        // ビルトインかどうか処理
        if (handle_builtin(buf)) {
            continue;
        }

        // 外部コマンド／バックグラウンド実行
        exec_command(buf);
    }

    // 履歴を書き出し
    save_history();

    // Raw モード解除
    disableRawMode();
    return 0;
}
