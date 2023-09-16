# cshell

Released: Sept 11, 2023

Duedate: Oct 2, 2023

Submitted: 

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

## Additional features

1. `history` - lists all commands run in the current session.

2. `!n` - runs the nth command in the history.

3. TAB completion - completes the command or path if there is only one option. If there are multiple options, it will list them. This already works for internal commands like cd, ls, etc because we are using readline(). But not for external like cat or grep. Also we should support wildcards.

4. `fg n` - brings the nth background command to the foreground.

5. .myshell - runs the commands in the file.

6. .myshellrc - is processed at the start of the shell. It contains configurations. For example, the prompt can be changed here.

7. `alias name command` - creates an alias for a command. For example, `alias l ls -l` will create an alias l for ls -l. `unalias name` will remove the alias.

8. `set var value` - sets the value of a variable. `unset var` will remove the variable.

9. `export var` - exports the variable to the environment. `unexport var` will remove the variable from the environment.

10. `setenv var value` - sets the value of an environment variable. `unsetenv var` will remove the environment variable.

11. `watchdog n command` - runs the command every n seconds. `watchdog off` will stop the watchdog.

12. `exit` - exits the shell.

13. `help` - lists all internal commands and their usage.

14. `help command` - lists the usage of the command.

15. Up and down arrows - cycles through the history.

16. .myshell_history - contains the history of all commands run in the shell. It is processed at the start of the shell. It is also updated at the end of the shell.

## Questions

1. How to handle the case where the user enters a command that doesn't exist? Should we print an error message? Or should we just ignore it?

2. Process IDs are the ones are shell assigns or the ones the OS assigns?

3. Is my code modular enough? Or should I break it down into more functions? Should I break into more files?

## TODO

- Command prompt: Fix hostname and test it works with changing dir with abs and rel paths.

- Change bg_pid to allow unlimited background commands.

- Currently uses `struct bgProcess bg_pid[MAX_BACKGROUND_PROCESSES];` to store background processes. This means that the number of background processes is limited to MAX_BACKGROUND_PROCESSES. Since we want unlimited processes we need dynamic memory allocation? We can either implement a linked list (adding a `struct bgProcess *next` pointer to the next bgProcess in each bgProcess) or use realloc(). Linked list will be slightly more complicated but will be more efficient. Realloc() will be easier to implement but will be less efficient.

- Test background functionality.

- Indicate when background commands terminate.

- Running commands in foreground and background: Test with multiple args.

- Test with given testing files (inf.c and args.c). Create some new testing files.

- Rewrite all comments.

- Bonus features.

- Test on linux.csc.uvic.ca