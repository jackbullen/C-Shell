#include "bg_processes.h"

node_t *head = NULL;

node_t *get_head() { return head; }

node_t *node_create(bg_process data) {
  node_t *new_node = (node_t *)malloc(sizeof(node_t));
  if (!new_node) {
    perror("Unable to allocate memory for new node");
    exit(1);
  }
  new_node->data = data;
  new_node->next = NULL;
  return new_node;
}

void node_clean(node_t *node) {
  free(node->data.name);
  free(node);
}

void bg_add(bg_process data) {
  node_t *new_node = node_create(data);
  new_node->next = head;
  new_node->data.name = strdup(data.name);
  new_node->data.state = strdup(data.state);
  head = new_node;
}

void bg_remove(int index) {
  if (index == 1) { // remove first
    node_t *tmp = head;
    head = head->next;
    node_clean(tmp);
    return;
  }

  node_t *node = bg_get(index - 1);

  if (node == NULL) // node does not exist
    return;

  node_t *tmp = node->next;
  node->next = tmp->next;
  node_clean(tmp);
}

void bg_clean() {
  node_t *node = head, *next = NULL;
  while (node != NULL) {
    next = node->next;

    if (kill(node->data.pid, SIGTERM) == -1) {
      if (errno == ESRCH) {
        printf("Process %d not found\n", node->data.pid);
      } else {
        perror("Error killing process");
      }
    }
    node_clean(node);
    node = next;
  }
}

void bg_print() {
  node_t *node = get_head();
  if (node == NULL) {
    printf("\nCurrently no background processes.\n\n");
    return;
  }

  // list all background processes with their state, pids, and name
  printf("\n");
  printf("%2s %6s   %10s    %4s\n", "#", "State", "Process ID", "Name");
  int index = 1;
  while (node != NULL) {
    printf("%2d   [%s]  %10d  %8s\n", index, node->data.state, 
            node->data.pid, node->data.name);
    node = node->next;
    index++;
  }
  printf("\n");
}

node_t *bg_get(int index) {
  if (index <= 0) {
    printf("\nError: Invalid process index.\n\n");
    return NULL;
  }

  node_t *node = get_head();
  int i = 1;
  while (node != NULL && i < index) {
    node = node->next;
    i++;
  }

  if (node == NULL) {
    printf("\nError: Process number %d does not exist.\nRun <bglist> to "
           "see active background processes. \nPass the process # to "
           "<bgkill>, not the process ID.\n\n",
           index);
    return NULL;
  }

  return node;
}

void bg_kill(int index) {
  node_t *node = bg_get(index);
  if (!node)
    return;

  pid_t node_pid = node->data.pid;
  bg_remove(index);
  kill(node_pid, SIGTERM);
}

void bg_pause(int index) {
  node_t *node = bg_get(index);
  if (!node)
    return;

  kill(node->data.pid, SIGSTOP);
  node->data.state = "P";
}

void bg_resume(int index) {
  node_t *node = bg_get(index);
  if (!node) 
    return;

  kill(node->data.pid, SIGCONT);
  node->data.state = "R";
}

void bg_execute(char **command) {
  if (command[0] == NULL) {
    return;
  }

  cid = fork();

  if (cid < 0) {
    perror("Fork failure.");
    exit(1);
  }

  if (cid == 0) {
    // Child process
    call_command(command[0], command);
    exit(0);
  }

  else {
    // Parent process
    bg_process new_process;
    new_process.pid = cid;
    new_process.state = "R";
    new_process.status = 0;
    new_process.name = command[0];
    bg_add(new_process);
  }
}

void *bg_monitor(void *arg) {
  int stat;
  int i = 0;
  while (1) {
    pthread_mutex_lock(&lock);
    node_t *node = get_head();
    while (node != NULL) {
      pid_t pid_test = waitpid(node->data.pid, &stat, WNOHANG);
      if (pid_test < 0) {
        perror("PID_TESTING ERROR:");
      }
      if (pid_test > 0) {
        printf("Process %d has terminated.\n", node->data.pid);
        bg_remove(i);
        break;
      } else {
        node = node->next;
        i++;
      }
    }
    pthread_mutex_unlock(&lock);
    sleep(1);
  }
  return NULL;
}