#
# Makefile for 'kill' command
#

BUILD = ../../../build

all:		kill

install:	kill
		mkdir -p $(BUILD)/cmds
		cp kill $(BUILD)/cmds

kill:		kill.c
		$(BUILD)/bin/lcc -A -o kill kill.c

clean:
		rm -f *~ kill
