#ifndef CONST_H
#define CONST_H

// 制御文字定義
#define ESC '\x1b'      // ANSI エスケープ開始
#define BACKSPACE 0x7f  // バックスペース
#define TAB '\t'       // タブ
#define CR '\r'        // キャリッジリターン
#define LF '\n'        // ラインフィード

// 定数
#define MAX_CMD_LENGTH 1024
#define MAX_PATH_LENGTH 1024
#define MAX_ARGS 64
#define MAX_HISTORY 100

// 色関連
#define COLOR_RESET "\033[39m"
#define COLOR_CYAN "\033[36m"
#define COLOR_GREEN "\033[32m"
#define COLOR_RED "\033[31m"

// プロンプト文字列
#define PROMPT "\n\033[36m[ tossh ]\033[32m$ \033[39m"

#endif // CONST_H