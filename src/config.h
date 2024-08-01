#ifndef CONFIG_H
#define CONFIG_H

#include "globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CONFIG_FILE "/.myshellrc"
#define MAX_LINE 512

extern const char *color_code;

void parse_color_code(const char *color);
void load_config();
void increment_color(char *incremented_color_code);
void change_prompt(const char *wd);

#endif