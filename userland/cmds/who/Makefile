#
# Makefile for 'who' command
#

BUILD = ../../../build

all:		who

install:	who
		mkdir -p $(BUILD)/cmds
		cp who $(BUILD)/cmds

who:		who.c
		$(BUILD)/bin/lcc -A -o who who.c

clean:
		rm -f *~ who
