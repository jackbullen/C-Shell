# cshell

Released: Sept 11, 2023

Duedate: Oct 2, 2023

Submitted:  September 30, 2023

## Overview

- Needs to work with all features on the `linux.csc.uvic.ca` server.

## Requirements

### Basic features

- Uses prompt: `username@hostname: /home/user >`

- Can run the following basic commands. All commands should support multiple arguments.

1. `ls` - with -l flag.
2. `cd` (will only take one argument). Should support absolute and relative paths. ~ for user's home dir should be supported.

### Background commands

- Unlimited background commands.

- `bg command_name [args]` will run the command in the background with args passed to it.

- bglist - lists all background commands with their PIDs and status.

## Additional features (implemented)

1. Polling thread - checks for background processes that have terminated. If a background process has terminated, it will print a message indicating that it has terminated within 1 second of it terminating.

2. TAB completion - completes the command or path if there is only one option. If there are multiple options, it will list them. This already works for internal commands like cd, ls, etc because we are using readline(). But not for external like cat or grep. Also we should support wildcards.

3. .myshellrc - is processed at the start of the shell. It contains configurations. For example, the prompt can be changed here.

4. Color bytes in prompt based on rcfile.

## Additional features (todo)

1. `history` - lists all commands run in the current session.

2. `bgstop <pid>` and `bgstart <pid>` bg processes

3. Up and down arrows - cycles through the history.

4. .myshell_history - contains the history of all commands run in the shell. It is processed at the start of the shell. It is also updated at the end of the shell.

## TODO

- start and stop

- history

- up and down arrows to cycle through history

- .myshell_history

## Issues

- The program does not support piping or redirection, history, aliases, or chaining.

- Memory leak observed in valgrind.