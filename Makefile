CC = gcc
CFLAGS = -Wall -g

# ソースファイル一覧
SRCS = main.c executor.c background.c signal_handler.c alias.c terminal.c

# オブジェクトファイル一覧
OBJS = $(SRCS:.c=.o)

# ターゲット
tossh: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# 共通コンパイルルール
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# クリーン
.PHONY: clean
clean:
	rm -f *.o tossh
