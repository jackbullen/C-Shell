# MyShell

## Overview

This document provides details on the MyShell program, a simple shell interpreter implementated in C. MyShell supports basic commands, background process management, configuration through an rc file, history, and custom tab completion.

## Dependencies

- GCC for compiler.
- Readline for user input.
- Standard C Libraries.

## Building

To build the project, navigate to the project root directory and run the provided Makefile.

```sh
make
```

Binary executables in the root directory.

Tests can be built by running the following command:

```sh
make test
```

Additional scripts are provided in the test directory to validate the program's functionality.

Clean up the generated files by running the following command:

```sh
make clean
```

## Running
Run the shell executable from the bin directory:

```sh
./myshell
```

Once the shell is running, interact with it by typing commands at the prompt.

## Commands

- `exit`: Exits the shell.
- `cd [dir]`: Changes the current directory to [dir].
- `bg [cmd]`: Executes [cmd] as a background process.
- `bglist`: Lists all background processes.
- `bgkill [n]`: Kills the background process numbered [n] in the bglist.
- `stop [n]`: Sends SIGSTOP to the background process numbered [n] in the bglist.
- `start [n]`: Sends SIGCONT to the background process numbered [n] in the bglist.
- `pstat [n]`: Prints the status of the background process numbered [n] in the bglist.
- `history [n]`: Prints the last [n] commands entered by the user.
- `!n`: Executes the nth command in the history.

## Issues/Notes

- The bg command does not perform error checking if a command is not passed to run in the background.

- The program supports a maximum of 5 background processes simultaneously.

- The program reads configuration from a file named .myshellrc located in the working directory.

- Memory allocation for strings, especially for state in bg_pid struct, needs to be reviewed and potentially revised to avoid using string literals directly.

- The program does not support piping or redirection, history, aliases, or chaining.