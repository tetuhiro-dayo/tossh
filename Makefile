CC = gcc
CFLAGS = -Wall
TARGET = tossh
OBJS = main.o alias.o terminal.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c alias.h terminal.h
	$(CC) $(CFLAGS) -c main.c

alias.o: alias.c alias.h
	$(CC) $(CFLAGS) -c alias.c

terminal.o: terminal.c terminal.h
	$(CC) $(CFLAGS) -c terminal.c

clean:
	rm -f $(TARGET) $(OBJS)
