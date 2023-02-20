#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */

/* Converts an integer to a string */
char *intToString(int number, int *arraySize)
{
    int counter;

    *arraySize = log10(number) + 1; 
    char *array = calloc(*arraySize, sizeof(char));
    
    for (counter = *arraySize - 1; counter >= 0; --counter, number /= 10)
    {
        array[counter] = (number % 10) + '0';
    }

    return array;
}

/* Checks to see if there was an error opening the specified file */
void checkOpenedFile(int fd)
{
    if (fd < 0)
    {
        printf("Problem opening file\n");
	exit(-1);
    }
}

/* Closes the specified file */
void closeFile(int fd)
{
    if (close(fd) < 0)
    {
	printf("Probem closing file");
	exit(-1);	
    }
}

/* Reads a char from the specified file */
void readChar(int fd, char *singleChar)
{
    int result = read(fd, singleChar, 1);
    if (result < 0)
    {
	printf("Problem reading from file");
	closeFile(fd);
	exit(-1);
    }
    else if (result == 0)
    {
	singleChar[0] = EOF;
    }
}

/* Writes chars to the specified file */
void writeChars(int fd, char *chars, int numChars)
{
    if (write(fd, chars, numChars) < 0)
    {
	printf("Problem writing to file");
	closeFile(fd);
	exit(-1);
    }
}

/* Returns the number of commands in the command history file */
int getNumCommands (char *fileName)
{
    int numCommands; 

    int fd;    
    char tempStorage[1];
  
    numCommands = 0;
    fd = open(fileName, O_RDONLY | O_CREAT, S_IRWXU);
    checkOpenedFile(fd);

    for (readChar(fd, tempStorage); *tempStorage != EOF; readChar(fd, tempStorage))
    {
	/* If we've reached a new line, then we've just read one command */
	if (*tempStorage == '\n')
	{
	    numCommands++;
	}
    }
    closeFile(fd);

    return numCommands;
}

/* Adds a command to the end of the command history file */
void writeCommand (char *fileName, char *args[])
{
    int fd;
    int commandNum;
    int argsCounter;
    int charCounter;
    char singleChar[1];

    char *numberString;
    int numberStringSize;

    /* Determine what command number this will be */
    commandNum = getNumCommands(fileName) + 1;
    numberString = intToString(commandNum, &numberStringSize);

    /* Go to the end of the file and write the command */
    fd = open(fileName, O_WRONLY | O_CREAT, S_IRWXU);
    checkOpenedFile(fd);
    lseek(fd, 0, SEEK_END);

    writeChars(fd, numberString, numberStringSize);
    *singleChar = '.';
    writeChars(fd, singleChar, 1);
    *singleChar = ' ';
    writeChars(fd, singleChar, 1);

    for (argsCounter = 0; args[argsCounter] != NULL; argsCounter++)
    {	
	for (charCounter = 0; args[argsCounter][charCounter] != '\0'; charCounter++);
	writeChars(fd, args[argsCounter], charCounter);
	writeChars(fd, singleChar, 1);
    }
    *singleChar = '\n';
    writeChars(fd, singleChar, 1);

    closeFile(fd);
    free(numberString);
}

/* Places the text of the most recent command entered into [inputBuffer] */
/* Returns the length of the line */
/* If no commands have been entered, returns -1 */
int getMostRecentCommandLine(char *fileName, char inputBuffer[]) 
{
    int length;

    int fd;
    int numCommands;
    int counter;
    char tempStorage[1];
    
    /* If no commands have been entered, return -1 */
    numCommands = getNumCommands(fileName);
    if (numCommands == 0)
    {
	printf("Oops! No commands have been entered yet, so can't fetch the most recent one.\n");
	return -1;
    }

    /* Find the location in the file where the most recent command is */
    fd = open(fileName, O_RDONLY | O_CREAT, S_IRWXU);
    checkOpenedFile(fd);
    for (counter = 0; counter < numCommands - 1; counter++)
    {	
	for (readChar(fd, tempStorage); *tempStorage != '\n'; readChar(fd, tempStorage));
    }

    /* Place the chars of the most recent command into [inputBuffer] and figure out the length */
    for (readChar(fd, tempStorage); *tempStorage != ' '; readChar(fd, tempStorage));
    length = 0;
    for (counter = 0; counter < MAX_LINE; counter++)
    {
	length++;
	readChar(fd, tempStorage);
	inputBuffer[counter] = *tempStorage;
	if (inputBuffer[counter] == '\n')
	{
	    break;
	}
    }
    closeFile(fd);

    return length;
}

/* Places the text of the most recent command entered that starts with [startingLetter] into [inputBuffer] */
/* Returns the length of the line */
/* If no commands have been entered or no recent command starting with [startingLetter] exists, returns -1 */
int getRecentCommandLine(char *fileName, char inputBuffer[], char startingLetter) 
{
    int length;

    int fd;
    int numCommands;
    int counter;
    char tempStorage[1];
    
    /* If no commands have been entered, return -1 */
    numCommands = getNumCommands(fileName);
    if (numCommands == 0)
    {
	printf("Oops! No commands have been entered yet, so can't fetch the most recent one.\n");
	return -1;
    }
   
    /* Find the location in the file where the 10 (or less) most recent commands are */
    fd = open(fileName, O_RDONLY | O_CREAT, S_IRWXU);
    checkOpenedFile(fd);
    for (counter = 0; counter < numCommands - 10; counter++)
    {
	for (readChar(fd, tempStorage); *tempStorage != '\n'; readChar(fd, tempStorage));
    }

    /* Place the most recent line starting with [startingLetter] into [inputBuffer] and figure out its length */
    length = -1;
    do
    {
	for (readChar(fd, tempStorage); *tempStorage != ' ' && *tempStorage != EOF; readChar(fd, tempStorage));
	readChar(fd, tempStorage);

	/* If this command starts with [startingLetter], put it into [inputBuffer] and find its length */
	if (*tempStorage == startingLetter)
	{
	    length = 1;
	    inputBuffer[0] = *tempStorage;
	    for (counter = 1; counter < MAX_LINE; counter++)
	    {
		length++;
		readChar(fd, tempStorage);
		inputBuffer[counter] = *tempStorage;
		if (inputBuffer[counter] == '\n')
		{
	    	    break;
		}
	    }
	}
	/* Otherwise, skip past it */
	else if (*tempStorage != EOF)
	{
	    for (readChar(fd, tempStorage); *tempStorage != '\n'; readChar(fd, tempStorage));
	}
    }
    while (*tempStorage != EOF);
    closeFile(fd);

    /* Make sure we actually found a command */
    if (length == -1)
    {
	printf("Oops! No recent commands start with that character, so can't fetch one.\n");
    }

    return length;
}

/* Prints out the 10 (or less) most recent commands in a nicely-formatted way */
void printRecentCommands(char *fileName)
{
    int length;

    int fd;
    int numCommands;
    int counter;
    char tempStorage[1];
    
    /* Make sure at least one command has been entered */
    /* NOTE: this technically isn't required as this function is only called when the user enters CNTRL-C; */
    /* According to Piazza, we can assume the user enters at least one command before using CNTRL-C */
    numCommands = getNumCommands(fileName);
    if (numCommands == 0)
    {
	return;
    }

    /* Find the location in the file where the 10 (or less) most recent commands are */
    fd = open(fileName, O_RDONLY | O_CREAT, S_IRWXU);
    checkOpenedFile(fd);
    for (counter = 0; counter < numCommands - 10; counter++)
    {
	for (readChar(fd, tempStorage); *tempStorage != '\n'; readChar(fd, tempStorage));
    }

    /* Print out all the recent commands */
    printf("\n\n-=-=-=-=-=-=-=-=-=-=-\n\n");
    for (readChar(fd, tempStorage); *tempStorage != EOF; readChar(fd, tempStorage))
    {
	printf("%c", *tempStorage);
    }

    printf("\n-=-=-=-=-=-=-=-=-=-=-\n\n");
    printf("^^ RECENT COMMANDS ^^\n");
    printf("  (by order of use)\n\n");
    printf("TIP: use the command \"r\" to execute the most recent command on the list\n");
    printf("TIP: you can specify one character after \"r\" that the command must start with\n\n");
}

