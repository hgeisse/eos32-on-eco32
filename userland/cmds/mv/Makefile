#
# Makefile for 'mv' command
#

BUILD = ../../../build

all:		mv

install:	mv
		mkdir -p $(BUILD)/cmds
		cp mv $(BUILD)/cmds

mv:		mv.c
		$(BUILD)/bin/lcc -A -o mv mv.c

clean:
		rm -f *~ mv
