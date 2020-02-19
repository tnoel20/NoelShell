
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_ARGS 256
#define COMMAND_BUFFER_SIZE 32768 
#define MAX_PATH_LEN 4096


/* Prototypes */
void changeDirectory(char* path, char* prevDirectory);
void launchJob(char** commandArgs, bool* detached);


int main(int argc, char* argv[])
{
    
    /* Sundry Strings */ 
    const char* userPrompt = "% ";
    const char* delim = " \n\r";
    const char* exitCmd = "exit";
    const char* detachSwitch = "&";
    const char* changeDir = "cd";

    /* Variable Declarations */
    char* commandArgs[MAX_ARGS]; /* Max number */
    char command[COMMAND_BUFFER_SIZE];
    char prevDirectory[MAX_PATH_LEN];
    int argCount = 0;
    int numArgs = 0;
    bool detached = false;

    /* Initialize prevDirectory */
    getcwd(prevDirectory, MAX_PATH_LEN);
    
    while(1) /* Repeat forever */
    {
        /* Display a prompt */
	fputs(userPrompt, stdout);

        /* Read a command from the user */
	fgets(command, COMMAND_BUFFER_SIZE, stdin);

        /* Parse the string into the command portion and
         * an array of pointers to argument strings using the blank
         * character as the separator. Note: the first argument
         * in the array of argument strings must be the same as
         * the command and the array must have a NULL pointer
         * after the last argument.
         */

	commandArgs[argCount] = strtok(command, delim);

	while (commandArgs[argCount])
	{
	    argCount++;
	    commandArgs[argCount] = strtok(NULL, delim);
	}

	/* Check to see if specified process is to be detached;
	 * negated because strcmp returns 0 if the strings match
	 * and, in this case, a match indicates detachment
	 */
	detached = !strcmp(commandArgs[argCount-1], detachSwitch);

	/* Don't want the detachSwitch to be passed
	 * as an argument if it is present.
	 */
	if (detached)
	{	
	    commandArgs[argCount-1] = NULL;
	    argCount--;
	}

	numArgs = argCount;

	/* Reset the argument counter */
	argCount = 0;

        /* If the user entered 'exit' then call the exit() system call
         * to terminate the process
         */
	if (!strcmp(commandArgs[0], exitCmd))
	{
	    exit(EXIT_SUCCESS);
	}


        if (strcmp(commandArgs[0], changeDir))
	{
	    launchJob(commandArgs, &detached);
	}
	else
	{
	    changeDirectory(commandArgs[1], prevDirectory);
	}

    } /* while */

} /* my shell */


void launchJob(char** commandArgs, bool* detached)
{   
    int childPid = 0;
    
    /* Fork a child process to execute the command and return 
     * the result of the fork() in the childPid variable so 
     * we know whether we're now executing as the parent 
     * or child and whether or not the fork() succeeded
     */
    
    // For any command other than "cd"
    childPid = fork();
        
    if (!childPid) /* We forked no child, we ARE the child */
    {
	/* We're now executing in the context of the child process.
	 * Use execvp() or execlp() to replace this program in 
	 * the process' memory with the executable command the 
	 * user has asked for.  
	 */
        
	 execvp(commandArgs[0], commandArgs);
        
	 /* Check to see if command was valid */
	 if (errno == ENOENT)
	 {
	    perror("Command not found");
	    exit(EXIT_FAILURE);
	 }	    
    }
    else if (childPid == -1) 
    {
	/* An error occured during the fork - print it */
	if (errno == ECHILD)
	{
	    perror("Could not fork child");
	}
        
    }
    else /* childPid is the PID of the child */
    {
	/* We're still executing in the parent process.
	 * Wait for the child to finish before we prompt
	 * again if the child process is not detached
	 */
	if (!*detached)
	{
	    waitpid(childPid, NULL, 0);
	}
	/* If the child process is detached, reset
	 * the detached flag for future children
	 * and print the job number of the detached child
	 */
	else
	{
	    printf("Job %d\n", childPid);
	    *detached = false;
	}
        
    }
    
}


void changeDirectory(char* path, char* prevDirectory)
{
    char currentDirectory[MAX_PATH_LEN];

    /* Adjust any known special path symbols */

    /* If path not prepended with '/', then prepend path with
     * the current working directory.
     */

    /* Change the current working directory (chdir()) to the amended
     * path string AND alter the PWD environment variable (setenv())
     */

    /* If the argument succeeding "cd" is "-" or if there is
     * no argument after "cd", then change directory to the
     * most previous directory
     */

    if (!path || *path == '-')
    {
        getcwd(currentDirectory, MAX_PATH_LEN);
        chdir(prevDirectory);
	setenv("PWD", prevDirectory, 1);
	strncpy(prevDirectory, currentDirectory, MAX_PATH_LEN);
    }
    else if (*(path) == '/')
    {
    
        getcwd(prevDirectory, MAX_PATH_LEN);
    
        chdir(path);
    
	setenv("PWD", path, 1);
    }

    //if (strcmp(path, ".."))
    //{
        
    //}
    
    else if (*(path) == '.')
    {
        getcwd(prevDirectory, MAX_PATH_LEN);
	strncat(prevDirectory, path+1, MAX_PATH_LEN-1);
        chdir(path);
	// 1 is an overwrite switch
	setenv("PWD", path, 1);
    }
    /* If the argument succeeding "cd" is "-" or if there is
     * no argument after "cd", then change directory to the
     * most previous directory
     */
    else
    {    
        printf("Consider it cd'd!\n");
    }

}
