#
# Makefile for 'stty' command
#

BUILD = ../../../../build

all:		stty

install:	stty
		mkdir -p $(BUILD)/cmds
		cp stty $(BUILD)/cmds

stty:		stty.c
		$(BUILD)/bin/lcc -A -o stty stty.c

clean:
		rm -f *~ stty
