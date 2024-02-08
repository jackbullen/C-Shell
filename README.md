# cshell

## About

### Basic feature

Infinite loop that let's user send commands with execvp.

### Background commands

- Background processes have their PID Structs stored in a linked list to support an arbitrary number of running background processes.

- `bg command_name [args]` will run the command in the background with args passed to it.

- bglist - lists all background commands with their PIDs and status.

- bgp - pauses a background process.

- bgr - resumes a background process.

### Additional features (implemented)

1. Polling thread - checks for background processes that have terminated. If a background process has terminated, it will print a message indicating that it has terminated within 1 second of it terminating.

2. .myshellrc - is processed at the start of the shell. It contains configurations.

### Additional features (todo)

1. `history` - lists all commands run in the current session.

2. Up and down arrows - cycles through the history.

3. .myshell_history - contains the history of all commands run in the shell. It is processed at the start of the shell. It is also updated at the end of the shell.

### Issues

- The program does not support piping or redirection, history, aliases, or chaining.