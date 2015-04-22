#
# Makefile for 'dof' command
#

BUILD = ../../../../build

all:		dof

install:	dof
		mkdir -p $(BUILD)/cmds
		cp dof $(BUILD)/cmds

dof:		dof.c
		$(BUILD)/bin/lcc -A -o dof dof.c

clean:
		rm -f *~ dof
