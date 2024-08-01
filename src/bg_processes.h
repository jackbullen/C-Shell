#ifndef BG_PROCESSES_H
#define BG_PROCESSES_H

#include "commands.h"
#include "globals.h"
#include <errno.h>
#include <signal.h>

extern pthread_mutex_t lock;

struct bg_process {
  pid_t pid;
  char *name;
  int status;
  char *state;
};

struct node {
  struct bg_process data;
  struct node *next;
};

struct node *get_head(void);
struct node *create_node(struct bg_process data);
void bg_add(struct bg_process data);
int bg_remove(pid_t pid);
void bg_clean();
void bg_print();
void bg_kill(int index);
void bg_pause(int index);
void bg_resume(int index);
void bg_execute(char **command);
void *bg_monitor(void *arg);

#endif