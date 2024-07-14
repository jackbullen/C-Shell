/*
 * main.c
 *
 * A simple shell.
 */
#include <errno.h>
#include <pthread.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "bg_processes.h"

#define CONFIG_FILE "/.myshellrc"
#define MAX_LINE 512

pthread_mutex_t lock;

void *monitor_bg_processes(void *arg) {
  int stat;
  while (1) {
    pthread_mutex_lock(&lock);
    struct Node *current = getHead();
    while (current != NULL) {
      pid_t pid_test = waitpid(current->data.pid, &stat, WNOHANG);
      if (pid_test < 0) {
        perror("PID_TESTING ERROR:");
      }
      if (pid_test > 0) {
        printf("Process %d has terminated.\n", current->data.pid);
        removeProcess(pid_test);
        break;
      } else {
        current = current->next;
      }
    }
    pthread_mutex_unlock(&lock);
    sleep(1);
  }
  return NULL;
}

const char *parsePS1(const char *color) {
  const char *color_code;
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

  return color_code;
}

const char *loadConfig(char *cwd) {
  char line[MAX_LINE];
  char config_path[1024];
  char value[1024] = "";

  strncpy(config_path, getenv("PWD"), sizeof(config_path) - 1);
  strcat(config_path, CONFIG_FILE);

  FILE *file = fopen(config_path, "r");
  if (!file) {
    perror("Failed to open config");
    return "";
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
  const char *color_code = parsePS1(value);
  fclose(file);
  return color_code;
}

void incrementColor(const char *color_code, char *incremented_color_code) {
  int color_value;
  sscanf(color_code, "\033[0;%dm", &color_value);

  color_value++;

  sprintf(incremented_color_code, "\033[0;%dm", color_value);
}

void changePrompt(char *cwd, const char *color_code, const char *uid,
                  const char *wd) {
  char incremented_color_code[16];
  incrementColor(color_code, incremented_color_code);

  strcat(cwd, color_code);
  strcat(cwd, uid);
  strcat(cwd, "\033[0m");
  strcat(cwd, "@");
  strcat(cwd, incremented_color_code);
  strcat(cwd, wd);
  strcat(cwd, "\033[0m");
  strcat(cwd, " % ");
}

void callCommand(char *command, char **commands, pid_t pid) {
  execvp(command, commands);
  perror("Call command Error ");
}

void handle_signal(int signal) {
  // Handle SIGINT and SIGTERM
  pthread_mutex_destroy(&lock);
  cleanUp(); // clear links
  exit(0);
}

int main(int argc, char *argv[]) {
  signal(SIGINT, handle_signal);
  signal(SIGTERM, handle_signal);

  // Initialize monitor thread mutex
  if (pthread_mutex_init(&lock, NULL) != 0) {
    perror("Failed to initialize mutex");
    exit(1);
  }

  // Setup background process monitor thread
  pthread_t monitor_thread;
  if (pthread_create(&monitor_thread, NULL, monitor_bg_processes, NULL) != 0) {
    perror("Failed to create monitor thread");
    exit(1);
  }

  // Setup command prompt.
  char cwd[255] = "";

  // Could modify this to load entire cwd and also other config settings
  // rather than setting uid and pwd after. Ie: cwd = loadConfig();
  const char *color_code = loadConfig(cwd);
  char incremented_color_code[16];
  incrementColor(color_code, incremented_color_code);

  const char *uid = getenv("USER");
  const char *pwd = getenv("PWD");

  char *lastSlash = strrchr(pwd, '/');
  if (lastSlash) {
    pwd = lastSlash + 1;
  }

  changePrompt(cwd, color_code, uid, pwd);

  pid_t cid;

  // Infinite for loop that keeps our shell running.
  for (;;) {
    char *cmd = readline(cwd);

    if (strlen(cmd) == 0) {
      continue;
    }

    char *input;
    char **command = malloc(8 * sizeof(char *));
    if (command == NULL) {
      perror("malloc failure");
      exit(1);
    }

    // Used for parsing input.
    int i = 0;

    input = strtok(cmd, " ");

    while (input != NULL) {
      command[i] = input;
      input = strtok(NULL, " ");
      i++;
    }
    command[i] = NULL;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // Command Definitions
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    // Exit.
    if (strcmp(command[0], "exit") == 0) {
      pthread_mutex_destroy(&lock);
      cleanUp();
      exit(0);
    }

    // Change dir.
    if (strcmp(command[0], "cd") == 0) {
      // cd got passed no dir...
      if (command[1] == NULL) {
        printf("\nError: No directory specified.\n\n");
        continue;
      }

      if (strcmp(command[1], "~") == 0) {
        memset(cwd, 0, sizeof(cwd));
        chdir(getenv("HOME"));
        changePrompt(cwd, color_code, uid, uid);
        continue;
      }

      // change dir and catch errors
      if (chdir(command[1]) < 0) {
        perror(command[1]);
      } else {
        memset(cwd, 0, sizeof(cwd));
        char newPrompt[1024];

        if (getcwd(newPrompt, sizeof(cwd)) == NULL) {
          perror("getcwd() error");
        } else {
          // update cwd

          char *lastSlash = strrchr(newPrompt, '/');
          if (lastSlash) {
            lastSlash++;
          } else {
            lastSlash = newPrompt;
          }

          changePrompt(cwd, color_code, uid, lastSlash);
        }
      }
      continue;
    }

    // List background processes. ie: print out bg_pid.
    if (strcmp(command[0], "bglist") == 0) {
      struct Node *current = getHead();
      if (current == NULL) {
        printf("\nCurrently no background processes.\n\n");
        continue;
      }
      printf("\n");
      printf("%2s %6s   %10s    %4s\n", "#", "State", "Process ID", "Name");
      int index = 1;
      while (current != NULL) {
        printf("%2d   [%s]  %10d  %8s\n", index, current->data.state,
               current->data.pid, current->data.name);
        current = current->next;
        index++;
      }
      printf("\n");
      continue;
    }

    // Kill a background process.
    // Takes in the counter of the process to kill from bglist, not the pid
    if (strcmp(command[0], "bgkill") == 0) {
      if (command[1] == NULL) {
        printf("\nError: Trying to kill an invalid process number.\nRun "
               "<bglist> to see active background processes.\n\n");
        continue;
      }

      int index = atoi(command[1]);
      if (index <= 0) {
        printf("\nError: Trying to kill an invalid process number.\nRun "
               "<bglist> to see active background processes.\n\n");
        continue;
      }

      struct Node *current = getHead();
      int currentIndex = 1;
      while (current != NULL && currentIndex < index) {
        current = current->next;
        currentIndex++;
      }

      if (current == NULL) {
        printf("\nError: Process number %d does not exist.\nRun <bglist> to "
               "see active background processes. \nPass the process # to "
               "<bgkill>, not the process ID.\n\n",
               index);
        continue;
      }

      kill(current->data.pid, SIGTERM);
      continue;
    }

    // Background Pause
    if (strcmp(command[0], "bgpause") == 0) {
      if (command[1] == NULL) {
        continue;
      }

      int index = atoi(command[1]);
      if (index <= 0) {
        continue;
      }

      struct Node *current = getHead();
      int currentIndex = 1;
      while (current != NULL && currentIndex < index) {
        current = current->next;
        currentIndex++;
      }

      if (current == NULL) {
        continue;
      }

      kill(current->data.pid, SIGSTOP);
      current->data.state = "S";
      continue;
    }

    // Background Resume
    if (strcmp(command[0], "bgr") == 0) {
      if (command[1] == NULL) {
        continue;
      }

      int index = atoi(command[1]);
      if (index <= 0) {
        continue;
      }

      struct Node *current = getHead();
      int currentIndex = 1;
      while (current != NULL && currentIndex < index) {
        current = current->next;
        currentIndex++;
      }

      if (current == NULL) {
        continue;
      }

      kill(current->data.pid, SIGCONT);
      current->data.state = "R";
      continue;
    }

    // Execute background process.
    if (strcmp(command[0], "bg") == 0) {
      // If no cmd is passed to run in bg.
      if (command[1] == NULL) {
        continue;
      }

      cid = fork();

      if (cid < 0) {
        perror("Fork failure.");
        exit(1);
      }

      if (cid == 0) {
        // Child process
        callCommand(command[1], command + 1, cid);
        exit(0);
      }

      else {
        // Parent process
        struct bgProcess newProcess;
        newProcess.pid = cid;
        newProcess.state = "R";
        newProcess.status = 0;
        newProcess.name = command[1];
        addProcess(newProcess);
      }
    } else {
      // Execute foreground process.
      cid = fork();
      if (cid < 0) {
        perror("Fork failure.");
        exit(1);
      }
      // For child process:
      if (cid == 0) {
        callCommand(command[0], command, cid);
        exit(0);
      }
      // For parent process:
      else {
        int status;
        waitpid(cid, &status, WUNTRACED);
      }
    }
    free(cmd);
    free(command);
  }
  pthread_mutex_destroy(&lock);
  return 0;
}