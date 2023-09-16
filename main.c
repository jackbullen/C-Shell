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

// For this version of assignment we want unlimited background processes.
#define MAX_BACKGROUND_PROCESSES 5

#define CONFIG_FILE "/.myshellrc"
#define MAX_LINE 512

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
	char *state;
};

// Global variables.
int counter = 0;
struct bgProcess bg_pid[MAX_BACKGROUND_PROCESSES];

/*
 * killProcess(pid to be removed)
 *
 * returns: 1 if success
 *          0 if failure
 *
 */
int killProcess(pid_t pid)
{
	for (int l = 0; l < counter; l++)
	{
		if (bg_pid[l].pid == pid)
		{
			for (int k = l; k < counter; k++)
			{
				bg_pid[k] = bg_pid[k + 1];
			}
			counter--;
			return 1;
		}
	}
	return 0;
}

const char* parsePS1(const char *color) 
{

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
	} else if (strcmp(color, "cyan") == 0) {
		color_code = "\033[0;36m";
	} else {
		color_code = "\033[0m";
	}

	return color_code;
}

const char* loadConfig(char *cwd) 
{
	char line[MAX_LINE];
	char config_path[1024];
	char value[1024] = "";

	strncpy(config_path, getenv("PWD"), sizeof(config_path) - 1);
	strcat(config_path, CONFIG_FILE);

	FILE *file = fopen(config_path, "r");
	if (!file) {
		perror("Failed to open config.");
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
	strcat(cwd, color_code);
	fclose(file);
	return color_code;
}


/*
 * Call bash commands using execvp.
 * This function is called from within forked
 * child processes to run our shells commands.
 */
void callCommand(char *command, char **commands, pid_t pid)
{
	execvp(command, commands);
	perror("Call command Error ");
}

int main(int argc, char *argv[])
{
	// Setup command prompt.
	char cwd[255] = "";

	// Could modify this to load entire cwd and also other config settings
	// rather than setting uid and pwd after. Ie: cwd = loadConfig();
	const char *color_code = loadConfig(cwd);

	char *uid = getenv("USER");
	char *pwd = getenv("PWD");
	
	strcat(strcat(cwd, uid), "@");
	strcat(strcat(cwd, pwd), "$ ");
	strcat(cwd, "\033[0m");

	pid_t pid_test;
	int stat;
	pid_t cid;

	// Infinite for loop that keeps our shell running.
	for (;;)
	{
		char *cmd = readline(cwd);
		if (strlen(cmd) == 0)
		{
			continue;
		}

		// After receieving input, check all bg processes for status changes. Update bg_pid accordingly.
		for (int l = 0; l < counter; l++)
		{
			if (WIFEXITED(bg_pid[l].status))
			{
				pid_test = waitpid(bg_pid[l].pid, &stat, WNOHANG);
				if (pid_test < 0)
				{
					perror("PID_TESTING ERROR: ");
				}

				if (pid_test > 0)
				{
					killProcess(pid_test);
					l--;
				}
			}
		}

		char *input;
		char **command = malloc(8 * sizeof(char *));
		if (command == NULL)
		{
			perror("malloc failure");
			exit(1);
		}

		int i = 0;

		input = strtok(cmd, " ");

		while (input != NULL)
		{
			command[i] = input;
			input = strtok(NULL, " ");
			i++;
		}
		command[i] = NULL;

		////////////////////////////////////////////////////////////////////////////////////////////////////
		// Command Definitions
		////////////////////////////////////////////////////////////////////////////////////////////////////

		// Exit.
		if (strcmp(command[0], "exit") == 0)
		{
			exit(1);
		}

		// Change dir.
		if (strcmp(command[0], "cd") == 0)
		{

			// cd got passed no dir...
			if (command[1] == NULL)
			{
				printf("\nError: No directory specified.\n\n");
				continue;
			}

			// change dir and catch errors
			if (chdir(command[1]) < 0)
			{
				perror(command[1]);
			}
			else
			{

				memset(cwd, 0, sizeof(cwd));
				char newPrompt[1024];
				if (getcwd(newPrompt, sizeof(cwd)) == NULL)
				{
					perror("getcwd() error");
				}
				else
				{
					// update cwd
					// snprintf(newPrompt, sizeof(newPrompt), "%s@%s$ ", uid, pwd);
					strcat(cwd, color_code);
					strcat(cwd, newPrompt);
					strcat(cwd, "$ ");
					strcat(cwd, "\033[0m");
				}
			}
			continue;
		}

		// List background processes. ie: print out bg_pid.
		if (strcmp(command[0], "bglist") == 0)
		{
			if (counter == 0)
			{
				printf("\nCurrently no background processes.\n\n");
				continue;
			}
			printf("\n");
			printf("%2s %6s   %10s\n", "#", "State", "Process ID");
			for (int j = 0; j < counter; j++)
			{
				printf("%2d   [%s]  %10d\n", j + 1, bg_pid[j].state, bg_pid[j].pid);
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

			// stop got passed no params...
			if (command[1] == NULL)
			{
				printf("\nError: Trying to kill an invalid process number.\nRun <bglist> to see active background processes.\n\n");
				continue;
			}

			// Check if it's stopped. If so, need to start before terminating.
			if (strcmp(bg_pid[atoi(command[1]) - 1].state, "S") == 0)
			{
				kill(bg_pid[atoi(command[1]) - 1].pid, SIGCONT);
				usleep(100);
			}

			kill(bg_pid[atoi(command[1]) - 1].pid, SIGTERM);
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

			// Stop got passed no params
			if (command[1] == NULL)
			{
				printf("\nError: Trying to stop an invalid process number.\nRun <bglist> to see active background processes.\n\n");
				continue;
			}

			// Stop got passed an invalid param
			if ((atoi(command[1]) > counter) || (atoi(command[1]) <= 0))
			{
				printf("\nError: Trying to stop an invalid process number.\nRun <bglist> to see active background processes.\n\n");
				continue;
			}

			// Check if this process is already stopped.
			if (strcmp(bg_pid[atoi(command[1]) - 1].state, "S") == 0)
			{
				printf("\nPrEocess %d with pid = %d is already stopped.\n\n", atoi(command[1]), bg_pid[atoi(command[1]) - 1].pid);
				continue;
			}

			// Send SIGSTOP to stop the process.
			kill(bg_pid[atoi(command[1]) - 1].pid, SIGSTOP);
			bg_pid[atoi(command[1]) - 1].state = "S";

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
				printf("\nError: Trying to start an invalid process number.\nRun <bglist> to see active background processes.\n\n");
				continue;
			}

			if ((atoi(command[1]) > counter) || (atoi(command[1]) <= 0))
			{
				printf("\nError: Trying to start an invalid process number.\nRun <bglist> to \
see active background processes.\nProcess numbers may have changed if any processes terminated.\n\n");
				continue;
			}

			if (strcmp(bg_pid[atoi(command[1]) - 1].state, "R") == 0)
			{
				printf("\nProcess %d with pid = %d is already running.\n\n", atoi(command[1]), bg_pid[atoi(command[1]) - 1].pid);
			}

			kill(bg_pid[atoi(command[1]) - 1].pid, SIGCONT);
			bg_pid[atoi(command[1]) - 1].state = "R";

			// Should we free cmd here?
			//
			//
			//
			free(cmd);
			continue;
		}

		// Execute background process.
		if (strcmp(command[0], "bg") == 0)
		{

			// If no cmd is passed to run in bg.
			if (command[1] == NULL)
			{
				continue;
			}

			// If we have max # of background processes.
			if (counter == 5)
			{
				continue;
			}

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
				callCommand(command[1], command + 1, cid);

				exit(1);
			}

			// For parent process:
			else
			{

				// Ensure the child process is running before we add it to our bg_pid struct.
				usleep(2600);

				// Setup the background process in our struct.

				// Look into using memory allocation rather than simply assigning string literals.
				//
				//
				//
				bg_pid[counter].pid = cid;
				bg_pid[counter].state = "R";
				counter++;
			}
		}

		else
		{ 
			// Execute foreground process.

			cid = fork();

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

		// cmd is from readline and command from mallod, which created memory
		// Therefore we must free them.
		free(cmd);
		free(command);
	}
	return 0;
}
