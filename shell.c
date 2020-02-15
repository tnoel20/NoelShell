
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

// Might need to modify this later
#define COMMAND_BUFFER_SIZE 25600
#define MAX_ARGS 256

int main(int argc, char* argv[])
{
    
    /* Constant declarations */ 
    const char* userPrompt = "% ";

    /* Variable declarations */
    char command[COMMAND_BUFFER_SIZE];
    char* commandArgs[MAX_ARGS]; /* Max number */
    int childPid = 0;
    int argCount = 0;
    int numArgs = 0;

    /* Sundry Strings */
    const char* delim = " \n\r";
    const char* exitCmd = "exit";

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

	numArgs = argCount;
	
	/* Reset the argument counter */
	argCount = 0;
	

        /* If the user entered 'exit' then call the exit() system call
         * to terminate the process
         */
	if (!strcmp(commandArgs[0], exitCmd))
	{
	    exit(0);
	}

	/*// START DEBUG
	for (int i = 0; i < numArgs; i++)
	{
	   printf("%s\n", commandArgs[i]); 
	}
	// END DEBUG*/ 


        /* Fork a child process to execute the command and return 
         * the result of the fork() in the childPid variable so 
         * we know whether we're now executing as the parent 
         * or child and whether or not the fork() succeeded
         */

	childPid = fork();

        if (!childPid) /* We forked no child, we ARE the child */
        {
            /* We're now executing in the context of the child process.
             * Use execvp() or execlp() to replace this program in 
             * the process' memory with the executable command the 
             * user has asked for.  
             */

	    execvp(commandArgs[0], commandArgs);

	    // TODO Use errno to check for execvp error	(FNFE)?
	    // MAYBE NOT NECESSARY??	    
        }
        else if (childPid == -1) 
        {
            /* An error occured during the fork - print it */

        }
        else /* childPid is the PID of the child */
        {
            /* We're still executing in the parent process.
             * Wait for the child to finish before we prompt
             * again.
             */

	    wait(NULL);

	    // TODO Implement detached process functionality;
	    // If detached, don't wait
        }

    } /* while */

} /* my shell */
