#include "config.h"
#include "globals.h"
#include <string.h>
#include <unistd.h>

void execute_and_wait(char **command);
void call_command(char *command, char **commands);
void change_dir(char *dir);