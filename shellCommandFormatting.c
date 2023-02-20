#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */

/* Reads in [inputBuffer], separating it into distinct tokens using whitespace as delimiters */
/* Sets the args parameter as a null-terminated string */
void inputToCommand(char inputBuffer[], char *args[], int *background, int length)
{
    int i;	/* loop index for accessing [inputBuffer] array */
    int start;	/* index where beginning of next command parameter is */
    int ct;	/* index of where to place the next parameter into args[] */

    ct = 0;
    start = -1;

    /* examine every character in [inputBuffer] */
    for (i = 0; i < length; i++) { 
        switch (inputBuffer[i]){
        case ' ':
        case '\t' :               /* argument separators */
            if(start != -1){
                args[ct] = &inputBuffer[start];    /* set up pointer */
                ct++;
            }
            inputBuffer[i] = '\0'; /* add a null char; make a C string */
            start = -1;
            break;
            
        case '\n':                 /* should be the final char examined */
            if (start != -1){
                args[ct] = &inputBuffer[start];     
                ct++;
            }
            inputBuffer[i] = '\0';
            args[ct] = NULL; /* no more arguments to this command */
            break;

        case '&':
            *background = 1;
            inputBuffer[i] = '\0';
            break;
            
        default :             /* some other character */
            if (start == -1)
                start = i;
	} 
    }    
    args[ct] = NULL; /* just in case the input line was > 80 */
}

/* Reads in the next command line and separates it into distinct tokens using inputToCommand() */
/* Returns 1 if the next command line was read successfully, 0 otherwise */
int setup(char inputBuffer[], char *args[], int *background)
{
    int length; /* # of characters in the command line */

    /* read what the user enters on the command line */
    length = read(STDIN_FILENO, inputBuffer, MAX_LINE);  

    if (length == 0) 
    {
        exit(0); 
    }             /* ^d was entered, end of user command stream */
    if (length < 0){
	return 0;
    }
    
    inputToCommand(inputBuffer, args, background, length);
    return 1;
} 

/* Returns 1 if this command is a request to execute a recent command and 0 otherwise */
int isRecentRequest (char *args[]) 
{
    int result = 0;

    /* Either must be only the letter 'r', or the letter 'r' followed by a single character */    
    if ((*args[0] == 'r' && strlen(args[0]) == 1) && (args[1] == NULL || (args[2] == NULL) && strlen(args[1]) == 1))
    {
	result = 1;
    }

    return result;
}

/* Replaces the command in args with the recent command that it specifies as well as sets up background for running tasks in the background */
/* Also prints out the command that was found if successful */
/* Requires that args contains correct information requesting a recent command */
/* Returns 1 if successful, and 0 otherwise */
int getRecentCommand (char *fileName, char inputBuffer[], char *args[], int *background)
{
    int length;
    char startingLetter;
    int count;

    /* If no starting letter is specified, just get the most recent command */
    if (args[1] == NULL)
    {
	length = getMostRecentCommandLine(fileName, inputBuffer);
    }
    /* Otherwise, try to get the most recent command that starts with the letter specified */
    else
    {
	length = getRecentCommandLine(fileName, inputBuffer, *args[1]);
    }
  
    if (length == -1)
    {
	return 0;
    }

    /* Echo the command to the screen so the user can see it */
    count = 0;
    while (inputBuffer[count] != '\n')
    {
	printf("%c", inputBuffer[count]);
	count++;
    }
    printf("%c", inputBuffer[count]);

    /* Get the args formatted correctly so we can execute this command */
    inputToCommand(inputBuffer, args, background, length);

    return 1;
}

