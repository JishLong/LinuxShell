#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "shellFileHandling.c"
#include "shellCommandFormatting.c"

#define MAX_LINE 80 			     /* 80 chars per line, per command, should be enough. */
#define HISTORY_FILE_NAME "long1964.history" /* The name of the file that will store all the user's commands */

/* Boolean used to determine whether this process is the one the user is currently interacting with */
/* Essentially used so that a bunch of nested shells don't become "active" once the user types CTRL-C */
int isActive;

/* Handles the CTRL-C signal interrupt */
void handle_SIGINT()
{
    /* If this shell process is the one the user is currently interacting with, print out all the recent commands */
    if (isActive)
    {
	printRecentCommands(HISTORY_FILE_NAME);		
    }
    /* Otherwise, go do something else. Stop messing up my program >:( */
    else
    {
	wait();
    }
}

/* Contains the main shell loop */
int main(void)
{
    int shouldTryCommand;	/* Acts as a boolean to determine whether the current determined command should attempt to be written to file and executed */
    
    char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
    char *args[MAX_LINE/2+1]; 	/* command line (of 80) has max of 40 arguments */
    int background;             /* equals 1 if a command is followed by '&' */
    
    pid_t pid, childId; 	/* the process IDs of the parent and its child */
    int childRc;		/* the child's return code */
     
    /* Set up the signal handler */
    struct sigaction handler;
    handler.sa_handler = handle_SIGINT; 
    sigaction(SIGINT, &handler, NULL);   

    /* The main shell loop - the program terminates normally inside of setup(), which is in shellCommandFormatting.c */
    while (1)
    {            
	/* This process is the one the user is currently interacting with */
	isActive = 1;

	/* Get the next command */
	background = 0;
	printf("COMMAND->");
        fflush(0);
        shouldTryCommand = setup(inputBuffer, args, &background);

	/* Determine whether this the user's input signifies a request to execute a recent command - if it is, then do so */
	if (isRecentRequest(args))
	{
	    /* Try to find the recent command the user is specifying */	    
	    if (getRecentCommand(HISTORY_FILE_NAME, inputBuffer, args, &background)) 
	    {
		writeCommand(HISTORY_FILE_NAME, args);
	    }
	    /* If we can't find it, nothing should be executed - we'll basically start over */
	    else
	    {
	    	shouldTryCommand = 0;
    	    }
	}
	else if (shouldTryCommand)
	{
	    writeCommand(HISTORY_FILE_NAME, args);
	}
	
	/* The steps are:
	   (1) fork a child process using fork()
	   (2) the child process will invoke execvp()
	   (3) if background == 0, the parent will wait, otherwise returns to the setup() function.
	*/
	fflush(0);
    	if (shouldTryCommand)
    	{
	    /* This process may not be the active one anymore if a nested shell is created.
	       If no such event occurs, the loop will just start over and this process will be set to active again */
	    isActive = 0;

	    pid = fork();
	    if (pid < 0)
	    {
	        printf("Problem creating a fork\n");
	    } 
	    else if (pid == 0) 
	    {
	        if (execvp(args[0], args) < 0) 
		{
	            printf("Problem executing child command\n");
		    exit(-1);
    	        }
	    } 
	    else if (background == 0)
	    {
	        childId = wait(&childRc);
	    }
    	}	
    }
}

