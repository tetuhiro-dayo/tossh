# TOSSH - Terminal Operating System Shell

## 📖 Overview

**TOSSH** is a simple, customizable shell written in C.  
It supports command execution, directory navigation, command history, input/output redirection, alias registration, and more — all with colorful prompts!

## 🚀 Features

-   Real-time character input (non-canonical mode)
-   Colorful prompts and error messages
-   Command history
-   Built-in `cd`, `alias`, and custom commands like `hello`, `ver`, `joke`, `fortune`
-   Input/Output redirection (`>`, `>>`, `<`)
-   Alias system (`alias ll=ls -l`)
-   Simple and modular code structure

## 🛠️ Build

```bash
make
```

## 🖥️ Usage

```bash
./tossh
```

## Example

```bash
[ tossh : ~/workspace ]$ hello
Hello, World!
[ tossh : ~/workspace ]$ alias ll=ls -l
[ tossh : ~/workspace ]$ ll
total 12
-rw-r--r--  1 user  group  2048 Apr 27  main.c
...
```

## 📂 Project Structure

tossh/  
├── main.c  
├── alias.h  
├── alias.c  
├── terminal.h  
├── terminal.c  
├── Makefile

## 📜 License

MIT License
