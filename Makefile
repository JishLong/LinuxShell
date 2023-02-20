######################################################################
# Lab 5 Makefile
######################################################################
# Based on Prof. Champion's Systems II Lab 0 Makefile shell.
######################################################################
# Global variables.
######################################################################

CC=gcc
LD=ld
WARNS=-Wall -pedantic -Wextra
CFLAGS=-g3 -std=gnu99 ${WARNS}
LIBS=-lm


# shell is to compile shell.c, the rest are handy things
all: clean tags headers shell


# This builds visual symbol (.vs) files and the header files.
headers: *.c tags
	./headers.sh

# Tags (for C code) are too handy not to keep up to date.
# This lets us use Control-] with vim (ctags command).
# Alternatively, we can use etags with emacs (etags command).
# Comment out the command that you're NOT using.
tags: *.c
#    ctags -R .
	etags -R .


# This is a link rule, we have a universal compile rule down below
# Output is the target of the rule : -o $@
# I want to link all of the dependencies: $^
shell: shell.o
	${CC} -g -o $@ $^ ${LIBS}

shell.o: shell.c
	${CC} -g -c $<

# This is our master compiler rule to generate .o files.
# It needs all 4 warnings (see WARNS variable defined above)
%.o:%.c *.h
	${CC} ${CFLAGS} -c $< -o $@

# Runs the shell executable
run: shell
	./shell

# Cleans up the space so things actually get compiled correctly
clean:
	rm -f *.h *.vs *.o
