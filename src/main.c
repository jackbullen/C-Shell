/*
 * main.c
 *
 * A simple shell.
 */

// TODO:
// 1. Address issue when exiting with or killing SIGSTOP process
// 2. Rather than having command functions return void, have them return status.

#include "bg_processes.h"
#include "commands.h"
#include "config.h"
#include "globals.h"
#include <readline/history.h>

const char *uid;
const char *pwd;
char cwd[255];
pid_t cid;
pthread_mutex_t lock;

void handle_exit_signal(int signal) {
  // Handle SIGINT and SIGTERM
  pthread_mutex_destroy(&lock);
  bg_clean();
  exit(0);
}

int main(int argc, char *argv[]) {
  signal(SIGINT, handle_exit_signal);
  signal(SIGTERM, handle_exit_signal);

  // Initialize monitor thread mutex
  if (pthread_mutex_init(&lock, NULL) != 0) {
    perror("Failed to initialize mutex");
    exit(1);
  }

  // Setup background process monitor thread
  pthread_t monitor_thread;
  if (pthread_create(&monitor_thread, NULL, bg_monitor, NULL) != 0) {
    perror("Failed to create monitor thread");
    exit(1);
  }

  // Setup command prompt.
  load_config();
  char incremented_color_code[16];
  increment_color(incremented_color_code);

  uid = getenv("USER");
  pwd = getenv("PWD");

  char *lastSlash = strrchr(pwd, '/');
  if (lastSlash) {
    pwd = lastSlash + 1;
  }

  change_prompt(pwd);

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

    /////////////////////////////////////////////////////////////////////////////////
    // Command Definitions

    // Exit shell
    if (strcmp(command[0], "exit") == 0) {
      pthread_mutex_destroy(&lock);
      bg_clean();
      exit(0);
    }

    // Change directory
    else if (strcmp(command[0], "cd") == 0)
      change_dir(command[1]);

    // List background processes
    else if (strcmp(command[0], "bglist") == 0)
      bg_print();

    // Kill background process
    else if (strcmp(command[0], "bgkill") == 0) {
      if (command[1] == NULL) {
        printf("\nError: No index passed.\nRun "
               "<bglist> to see active background processes.\n\n");
        continue;
      }

      int index = atoi(command[1]);
      bg_kill(index);
    }

    // Pause background process
    else if (strcmp(command[0], "bgp") == 0) {
      if (command[1] == NULL) {
        printf("\nError: No index passed.\nRun "
               "<bglist> to see active background processes.\n\n");
        continue;
      }

      int index = atoi(command[1]);
      bg_pause(index);
    }

    // Resume background process
    else if (strcmp(command[0], "bgr") == 0) {
      if (command[1] == NULL) {
        printf("\nError: No index passed.\nRun "
               "<bglist> to see active background processes.\n\n");
        continue;
      }

      int index = atoi(command[1]);
      bg_pause(index);
    }

    // Execute background process
    else if (strcmp(command[0], "bg") == 0)
      bg_execute(command + 1);

    // Execute foreground process
    else
      execute_and_wait(command);
    /////////////////////////////////////////////////////////////////////////////////

    free(cmd);
    free(command);
  } // shell loop
}