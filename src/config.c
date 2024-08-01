#include "config.h"

const char *color_code;

void parse_color_code(const char *color) {
  if (strcmp(color, "red") == 0) {
    color_code = "\033[0;31m";
  } else if (strcmp(color, "green") == 0) {
    color_code = "\033[0;32m";
  } else if (strcmp(color, "yellow") == 0) {
    color_code = "\033[0;33m";
  } else if (strcmp(color, "blue") == 0) {
    color_code = "\033[0;34m";
  } else if (strcmp(color, "magenta") == 0) {
    color_code = "\033[0;35m";
  } else {
    color_code = "\033[0m";
  }
}

void load_config() {
  char line[MAX_LINE];
  char config_path[1024];
  char value[1024] = "";

  strncpy(config_path, getenv("PWD"), sizeof(config_path) - 1);
  strcat(config_path, CONFIG_FILE);

  FILE *file = fopen(config_path, "r");
  if (!file) {
    perror("Failed to open config");
    return;
  }

  while (fgets(line, sizeof(line), file)) {
    char *setting = strtok(line, "=");
    char *config_value = strtok(NULL, "=");

    if (config_value) {
      config_value[strcspn(config_value, "\n")] = 0;
    }

    if (setting && config_value) {
      if (strcmp(setting, "PS1") == 0) {
        strncpy(value, config_value, sizeof(value) - 1);
      }
    }
  }
  parse_color_code(value);
  fclose(file);
}

void increment_color(char *incremented_color_code) {
  int color_value;
  sscanf(color_code, "\033[0;%dm", &color_value);

  color_value++;

  sprintf(incremented_color_code, "\033[0;%dm", color_value);
}

void change_prompt(const char *wd) {
  char incremented_color_code[16];
  increment_color(incremented_color_code);

  strcat(cwd, color_code);
  strcat(cwd, uid);
  strcat(cwd, "\033[0m");
  strcat(cwd, "@");
  strcat(cwd, incremented_color_code);
  strcat(cwd, wd);
  strcat(cwd, "\033[0m");
  strcat(cwd, " % ");
}