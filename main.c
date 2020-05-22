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


/*                                                                    
 * Background process.
 *                                                                    
 * Holds pid of background child processs, it's status, and it's state (running or stopped).
 *                                                                    
 */
struct bgProcess
{
  pid_t pid;
  int status;
  char* state;
};

// Global variables.
int counter=0;
struct bgProcess bg_pid[5];

/*
 * killProcess(pid to be removed)
 *
 * returns: 1 if success
 *          0 if failure
 *
 */
int killProcess(pid_t pid)
{
  for (int l = 0; l<counter; l++)
    {
      if (bg_pid[l].pid == pid)
	{
	  for (int k = l; k < counter; k++)
	    {
	      bg_pid[k] = bg_pid[k+1];
		    }
	  counter--;
	  return 1;
	}
    }
  return 0;
}

/*
 * Call bash commands using execvp.
 * This function is called from within forked
 * child processes to run our shells commands.
 */
void callCommand(char * command, char ** commands, pid_t pid)
{
  execvp(command, commands);
  perror("Call command Error ");
}

/*
Not being used because I can just check for dead children at start of my for(;;) 
Still here because I don't fully understand how it works and want to learn.

 * Handler for SIGCHLD. Decrement counter when
 * child processes terminate.
 */
/*
void sigchldHandler(int sig)
{
  int status;
  pid_t childPid;
  while((childPid = waitpid(-1, &status, WNOHANG) > 0))
    {
      printf("Sig chld handler popping off: %d %d",sig, childPid);
      killProcess(childPid);
    }
}
*/
  


int main (int argc, char * argv[])
{

  // Setup SIGCHLD handler.
  // struct sigaction sa;
  // memset(&sa, 0 ,sizeof(sa));
  //  sa.sa_handler = sigchldHandler;

  // Get env username and setup var for CWD of our shell.
  char cwd[255];
  char *uid=getenv("USER");
  char *pwd;


  pwd=getenv("PWD");
  strcat(strcat(cwd,uid),"@");
  strcat(strcat(cwd,pwd),"$ ");
  
  
  pid_t pid_test;
  int stat;
  
  pid_t cid;

  // Infinite for loop that keeps our shell running.
         for (;;)
	  {
	    
	    // Reset CWD so it doesn't self-concatenate after each cmd...
	    //	    memset(cwd, 0, 255);
	    /*
	      if (sigaction(SIGCHLD, &sa, NULL) == -1)
	      {
	      perror("sigaction");
	      return 1;
	      }
	    */
	    // Create CWD pathname.
	    /*pwd=getenv("PWD");
	    strcat(strcat(cwd,uid),"@");
	    strcat(strcat(cwd,pwd),"$ ");
	    */
	    // Shell prompt for input.
	    char 	*cmd = readline (cwd);
	    if (strlen(cmd) == 0) {
	      continue;
	    }
	    

	    for (int l = 0; l < counter; l++)
	      {
		if (WIFEXITED(bg_pid[l].status))
		  {
		    pid_test = waitpid(bg_pid[l].pid, &stat, WNOHANG);
		    if (pid_test < 0)
		      {
			perror("PID_TESTING ERROR: ");
		      }
		    
		    
		    if (pid_test>0)
		      {
			killProcess(pid_test);
			l--;
		      }
		  }
	      }
 
	    char * input;
	    char ** command = malloc(8 * sizeof(char *));
	    if( command == NULL) {
	      perror("malloc failure");
	      exit(1);
	    }
	    
	    int i = 0;
	    
	    input = strtok(cmd," ");
	    
	    while (input != NULL)
	      {
		command[i] = input;
		input = strtok(NULL, " ");
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
	    
	    
	    
	    
	    //   Problem: Creates bgProcess even if execvp fails...
	    //
	    //
	    //
	    ////
	    //
	    //  
	    //
	    ////
	    
	    // Print background processes.
	    if (strcmp(command[0],"bglist") == 0)
	      {
		if (counter == 0)
		  {
		    printf("\nCurrently no background processes.\n\n");
		    continue;
		  }
		printf("\n");
		printf("%2s %6s   %10s\n","#","State","Process ID");
		for (int j=0;j<counter;j++)
		  {
		    printf("%2d   [%s]  %10d\n", j+1, bg_pid[j].state,bg_pid[j].pid);
		  }
		printf("\n");
		continue;
	      }
	    
	    
	    // Kill a background process.
	    if (strcmp(command[0], "bgkill") == 0)
	      {

		if (counter == 0)
                  {
                    printf("\nCurrently no background processes.\n\n");
                    continue;
                  }
		
		// Check if it's stopped. If so, need to start before terminating.
		if (strcmp(bg_pid[atoi(command[1])-1].state,"S") == 0)
		  {
		    kill(bg_pid[atoi(command[1])-1].pid, SIGCONT);
		    usleep(100);
		  }
		
		kill(bg_pid[atoi(command[1])-1].pid, SIGTERM); 
		continue;
	      }
	    
	    
	    // Send SIGSTOP
	    if (strcmp(command[0], "stop") == 0)
	      {
		if (counter == 0)
                  {
                    printf("\nCurrently no background processes.\n\n");
                    continue;
                  }
		
		// stop got passed no params...
		if (command[1] == NULL)
		  {
		    printf("\nError: Trying to stop an invalid process number.\nRun <bglist> to see active background processes.\n\n");
		    continue;
		  }
		
		if ((atoi(command[1])>counter) || (atoi(command[1])<=0))
		  {
		    printf("\nError: Trying to stop an invalid process number.\nRun <bglist> to see active background processes.\n\n");
		    continue;
		  }

		if (strcmp(bg_pid[atoi(command[1])-1].state,"S") == 0)
                  {
                    printf("\nProcess %d with pid = %d is already stopped.\n\n",atoi(command[1]),bg_pid[atoi(command[1])-1].pid);
                  }
		
		kill(bg_pid[atoi(command[1])-1].pid, SIGSTOP);
		bg_pid[atoi(command[1])-1].state = "S";
		
		continue;
	      }
	    
	    
	    // Send SIGCONT
	    if (strcmp(command[0], "start") == 0)
	      {
		if (counter == 0)
                  {
                    printf("\nCurrently no background processes.\n\n");
                    continue;
                  }

		if (command[1] == NULL)
                  {
                    printf("\nError: Trying to stop an invalid process number.\nRun <bglist> to see active background processes.\n\n");
                    continue;
                  }
		
		if ((atoi(command[1])>counter) || (atoi(command[1])<=0))
                  {
                    printf("\nError: Trying to start an invalid process number.\nRun <bglist> to \
see active background processes.\nProcess numbers may have changed if any processes terminated.\n\n");
                    continue;
                  }

		if (strcmp(bg_pid[atoi(command[1])-1].state,"R") == 0)
		  {
		    printf("\nProcess %d with pid = %d is already running.\n\n",atoi(command[1]),bg_pid[atoi(command[1])-1].pid);
		  }
		
		kill(bg_pid[atoi(command[1])-1].pid, SIGCONT);
		bg_pid[atoi(command[1])-1].state = "R";
		free(cmd);
		continue;
	      }
	    
	    // Execute background process.
	    if (strcmp(command[0], "bg") == 0)
	      {
		
		// If no cmd is passed to run in bg.
		if (command[1] == NULL) {continue;}
		
		// If we have max # of background processes.
		if (counter==5) {continue;}
		
		
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
		    callCommand(command[1], command+1, cid);
		    
		    exit(1);
		  }
		
		// For parent process:                                                                     
		else
		  {
		  
		  /* the value 2600 was chosen from trial and error.
		   * it is more time than it takes for the child process to
		   * run the command ls. which is significantly more than it takes
		   * for the child to give errors. (~1000micro seconds).
		   ***********************************************************************
		   * we sleep so that our parent (where we are now) will arrive at the top of the for(;;)
		   * and take our next command input (also displaying our cwd and username)
		   * AFTER the ls command, or hopefully w.e. "relatiely" short background cmd.
		   * BUT ... u have to ask ... why would you run a short command like that in
		   * the background.
		   */
		    usleep(2600);
		    
		    
		    // Setup the background process in our struct.
		    bg_pid[counter].pid = cid;
		    bg_pid[counter].state = "R";
		    counter++;
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
		  callCommand(command[0], command, cid);
		  exit(1);
		}
	      
	      // For parent process:
	      else
		{
		  waitpid(cid, &bg_pid[counter].status, WUNTRACED);
		}
	    }
	    
	    free (input);
	    free (cmd);
	    free (command);
	    //continue;
	  }
	//if (WIFEXITED(stat)) {
	// printf("exited, status=%d\n",WEXITSTATUS(stat));
	//}
	
	return 0;
	
}
