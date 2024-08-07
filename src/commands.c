#include "commands.h"

void execute_and_wait(char **command) {
  cid = fork();
  if (cid < 0) {
    perror("Fork failure.");
    exit(1);
  }

  if (cid == 0) { // child
    call_command(command[0], command);
    exit(0);
  }

  else { // parent
    int status;
    waitpid(cid, &status, WUNTRACED);
  }
}

void call_command(char *file, char **commands) {
  execvp(file, commands);
  perror("Call command Error ");
}

void change_dir(char *dir) {
  // cd got passed no dir...
  if (dir == NULL) {
    printf("\nError: No directory specified.\n\n");
    return;
  }

  // TODO: handle subdirs of ~
  //       for now just move to ~
  if (dir[0] == '~') {
    memset(cwd, 0, sizeof(*cwd));
    chdir(getenv("HOME"));
    change_prompt(uid);
    return;
  }

  // change dir and catch errors
  if (chdir(dir) < 0) {
    perror(dir);
  } else {
    memset(cwd, 0, sizeof(*cwd));
    char new_prompt[1024];

    if (getcwd(new_prompt, sizeof(cwd)) == NULL) {
      perror("getcwd() error");
    } else {
      // get the directory name cwd
      char *directory = strrchr(new_prompt, '/');
      if (directory) {
        directory++;
      } else {
        directory = new_prompt;
      }

      change_prompt(directory);
    }
  }
}