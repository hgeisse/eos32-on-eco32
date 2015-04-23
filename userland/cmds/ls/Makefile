#
# Makefile for 'ls' command
#

BUILD = ../../../build

all:		ls

install:	ls
		mkdir -p $(BUILD)/cmds
		cp ls $(BUILD)/cmds

ls:		ls.c
		$(BUILD)/bin/lcc -A -o ls ls.c

clean:
		rm -f *~ ls
