#
# Makefile for 'ed' command
#

BUILD = ../../../../build

all:		ed

install:	ed
		mkdir -p $(BUILD)/cmds
		cp ed $(BUILD)/cmds

ed:		ed.c
		$(BUILD)/bin/lcc -A -o ed ed.c

clean:
		rm -f *~ ed
