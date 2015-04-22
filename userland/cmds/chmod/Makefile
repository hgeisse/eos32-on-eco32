#
# Makefile for 'chmod' command
#

BUILD = ../../../../build

all:		chmod

install:	chmod
		mkdir -p $(BUILD)/cmds
		cp chmod $(BUILD)/cmds

chmod:		chmod.c
		$(BUILD)/bin/lcc -A -o chmod chmod.c

clean:
		rm -f *~ chmod
