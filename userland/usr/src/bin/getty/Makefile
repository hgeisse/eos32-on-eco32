#
# Makefile for 'getty' command
#


all:		getty

install:	getty
		mkdir -p $(BUILD)/cmds
		cp getty $(BUILD)/cmds

getty:		getty.c
		/bin/lcc -A -o getty getty.c

clean:
		rm -f *~ getty
