#ifndef BG_PROCESSES_H
#define BG_PROCESSES_H

#include "commands.h"
#include "globals.h"
#include <errno.h>
#include <signal.h>

extern pthread_mutex_t lock;

typedef struct bg_process {
  pid_t pid;
  char *name;
  int status;
  char *state;
} bg_process;

typedef struct node_t {
  struct bg_process data;
  struct node_t *next;
} node_t;

node_t *get_head(void);
node_t *node_create(bg_process data);
void node_clean(node_t *node);

void bg_add(bg_process data);
void bg_clean();
void bg_print();
void bg_kill(int index);
node_t *bg_get(int index);
void bg_pause(int index);
void bg_resume(int index);
void bg_execute(char **command);
void *bg_monitor(void *arg);

#endif