/*
 * main.c
 *
 * A simple shell.
 */
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <readline/readline.h>

int counter=0;


/*
 * Call bash commands using execvp.
 * This function is called from within forked
 * child processes to run our shells commands.
 */
void callCommand(char * command, char ** commands)
{
	if (execvp(command, commands) < 0)
		    {
		      perror(command);
		      exit(1);
		    }
}

/*
 * Handler for SIGCHLD. Decrement counter when
 * child processes terminate.
 */
void sigchldHandler(int sig)
{
  int status;
  pid_t childPid;
  while((childPid = waitpid(-1, &status, WNOHANG) > 0))
    {
      counter--;
    }
}
    
  


int main (int argc, char * argv[])
{

  // Setup SIGCHLD handler.
  struct sigaction sa;
  memset(&sa, 0 ,sizeof(sa));
  sa.sa_handler = sigchldHandler;

  // Get env username and setup var for CWD of our shell.
  char cwd[255];
  char *uid=getenv("USER");
  char *pwd;

  pid_t cid;
  pid_t bg_pid[5];

  int status[5];


  // Infinite for loop that keeps our shell running.
	for (;;)

	{
	  // Reset CWD so it doesn't self-concatenate after each cmd...
	  memset(cwd, 0, 255);

	  if (sigaction(SIGCHLD, &sa, NULL) == -1)
	    {
	      perror("sigaction");
	      return 1;
	    }
	  
	  // Create CWD pathname.
	  pwd=getenv("PWD");
	  strcat(strcat(cwd,uid),"@");
	  strcat(strcat(cwd,pwd),"$ ");
	  
	  // Shell prompt for input.
	  char 	*cmd = readline (cwd);
	  if (strlen(cmd) == 0) {
	    continue;
	  }
	  

	  char * pch;
	  char ** command = malloc(8 * sizeof(char *));
	  if( command == NULL) {
	    perror("malloc failure");
	    exit(1);
	  }
	  
	  int i = 0;
	  
	  pch = strtok(cmd," ");
	  
	  while (pch != NULL)
	    {
	      command[i] = pch;
	      pch = strtok(NULL, " ");
	      i++;
	    }
	  command[i] = NULL;
	  // Exit.
	  if (strcmp(command[0], "exit") == 0)
	    {
	      exit(1);
	    }
	  
	  // Change dir.
	  if (strcmp(command[0], "cd") == 0)
	    {
	      if (chdir(command[1])<0)
		{
		  perror(command[1]);
		}
	      continue;
	    }
	  

	  // Print background processes.
	  if (strcmp(command[0],"bglist") == 0)
	  {                
	    for (int j=0;j<counter;j++)
	  	{
			printf("%d\n",bg_pid[j]);
		}
	    continue;
	  }

	  // Kill a background process.
	  if (strcmp(command[0], "bgkill") == 0)
	  {
		  kill((pid_t)command[1], SIGKILL);
		  continue;
	  }
	  
	  // Execute background process.
	  if (strcmp(command[0], "bg") == 0)
	    {
	      cid = fork();
	      
	      // Error handling for fork failure.                                                    
	      if (cid < 0)
		{
		  perror("Fork failure.");
		  exit(1);
		}
	      
	      // For child process:                                                                      
	      if (cid == 0)
		{
			// Call the second command (command[1]), because command[0] is bg.
			callCommand(command[1], command+1);
		}
	      
	      // For parent process:                                                                     
	      else
		{
		  bg_pid[counter] = cid;
		  counter++;
		  printf("%d", counter);
		  while ((cid = waitpid(-1, &status[counter], WNOHANG)) > 0)
		    {}
		}
	      
	    }
	  
	  
	  else { // Not a background command.
	    
	    cid = fork();
	    // Error handling for fork failure.
	    if (cid < 0)
	      {
		perror("Fork failure.");
		exit(1);
	      }
	    
	    // For child process:
	    if (cid == 0)
	      {
			callCommand(command[0], command);
	      }
	    
	    // For parent process:
	    else
	      {
		waitpid(cid, &status[counter], WUNTRACED);
	      }
	  }
	  free (pch);
	  free (cmd);
	  free (command);
	}
	return 0;
}
