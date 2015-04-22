#
# Makefile for 'ps' command
#

BUILD = ../../../../build

all:		ps

install:	ps
		mkdir -p $(BUILD)/cmds
		cp ps $(BUILD)/cmds

ps:		ps.c
		$(BUILD)/bin/lcc -A -o ps ps.c

clean:
		rm -f *~ ps
