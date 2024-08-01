#include "bg_processes.h"

struct node *head = NULL;

struct node *get_head() { return head; }

struct node *create_node(struct bg_process data) {
  struct node *new_node = (struct node *)malloc(sizeof(struct node));
  if (!new_node) {
    perror("Unable to allocate memory for new node");
    exit(1);
  }
  new_node->data = data;
  new_node->next = NULL;
  return new_node;
}

void bg_add(struct bg_process data) {
  struct node *new_node = create_node(data);
  new_node->next = head;
  new_node->data.name = strdup(data.name);
  new_node->data.state = strdup(data.state);
  head = new_node;
}

int bg_remove(pid_t pid) {
  struct node *current = head, *prev = NULL;
  while (current != NULL) {
    if (current->data.pid == pid) {
      if (prev == NULL) {
        head = current->next;
      } else {
        prev->next = current->next;
      }
      free(current->data.name);
      free(current);
      return 1;
    }
    prev = current;
    current = current->next;
  }
  return 0;
}

void bg_clean() {
  struct node *current = head, *next = NULL;
  while (current != NULL) {
    next = current->next;

    if (kill(current->data.pid, SIGTERM) == -1) {
      if (errno == ESRCH) {
        printf("Process %d not found\n", current->data.pid);
      } else {
        perror("Error killing process");
      }
    }
    free(current->data.name);
    free(current->data.state);
    free(current);
    current = next;
  }
}

void bg_print() {
  struct node *current = get_head();
  if (current == NULL) {
    printf("\nCurrently no background processes.\n\n");
    return;
  }

  // list all background processes with their state, pids, and name
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
}

void bg_kill(int index) {
  if (index <= 0) {
    printf("\nError: Trying to kill an invalid process number.\nRun "
           "<bglist> to see active background processes.\n\n");
    return;
  }

  struct node *current = get_head();
  int i = 1;
  while (current != NULL && i < index) {
    current = current->next;
    i++;
  }

  if (current == NULL) {
    printf("\nError: Process number %d does not exist.\nRun <bglist> to "
           "see active background processes. \nPass the process # to "
           "<bgkill>, not the process ID.\n\n",
           index);
    return;
  }

  kill(current->data.pid, SIGTERM);
}

void bg_pause(int index) {
  if (index <= 0) {
    printf("\nError: Invalid process to pause.\n\n");
    return;
  }

  struct node *current = get_head();
  int i = 1;
  while (current != NULL && i < index) {
    current = current->next;
    i++;
  }

  if (current == NULL) {
    printf("\nError: Less than %d active processes.\n\n", index);
    return;
  }

  kill(current->data.pid, SIGSTOP);
  current->data.state = "P";
}

void bg_resume(int index) {
  if (index <= 0) {
    printf("\nError: Invalid process to pause.\n\n");
    return;
  }

  struct node *current = get_head();
  int i = 1;
  while (current != NULL && i < index) {
    current = current->next;
    i++;
  }

  if (current == NULL) {
    printf("\nError: Less than %d active processes.\n\n", index);
    return;
  }

  kill(current->data.pid, SIGCONT);
  current->data.state = "R";
}

void bg_execute(char **command) {
  // If no cmd is passed to run in bg.
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
    call_command(command[0], command, cid);
    exit(0);
  }

  else {
    // Parent process
    struct bg_process new_process;
    new_process.pid = cid;
    new_process.state = "R";
    new_process.status = 0;
    new_process.name = command[1];
    bg_add(new_process);
  }
}

void *bg_monitor(void *arg) {
  int stat;
  while (1) {
    pthread_mutex_lock(&lock);
    struct node *current = get_head();
    while (current != NULL) {
      pid_t pid_test = waitpid(current->data.pid, &stat, WNOHANG);
      if (pid_test < 0) {
        perror("PID_TESTING ERROR:");
      }
      if (pid_test > 0) {
        printf("Process %d has terminated.\n", current->data.pid);
        bg_remove(pid_test);
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