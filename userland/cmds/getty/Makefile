#
# Makefile for 'getty' command
#

BUILD = ../../../build

all:		getty

install:	getty
		mkdir -p $(BUILD)/cmds
		cp getty $(BUILD)/cmds

getty:		getty.c
		$(BUILD)/bin/lcc -A -o getty getty.c

clean:
		rm -f *~ getty
