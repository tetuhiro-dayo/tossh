#ifndef INPUT_H
#define INPUT_H

#include <stddef.h>

/**
 * readLine: 1文字ずつ読み込みつつ行編集を行い、buf に格納する
 * @param buf: コマンド入力用バッファ
 * @param maxlen: buf の最大長
 * @return 読み込んだ文字数（改行を除く）。EOF 等で負値を返す。
 */
int readLine(char *buf, size_t maxlen);

#endif // INPUT_H