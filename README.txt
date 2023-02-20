Josh Long
CSE 2431
11-20-22

To compile this program, one can simply type "make" into the command line. Be careful of any .h, .vs, and .o files in the vicinity, as they will get deleted right before compilation to ensure that fresh files are generated from the source code.

To run this program, typing "make run" into the command line is an option. Simply typing "./shell" is also sufficient.

IMPORTANT:

This program relies on a file that is created during runtime to work. The name of this file is "long1964.history", and it's critical that this file isn't used for other purposes (i.e. this program should be the only entity that modifies the file - it must be formatted in a certain way).
