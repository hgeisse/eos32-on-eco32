#
# Makefile for 'sync' command
#

BUILD = ../../../build

all:		sync

install:	sync
		mkdir -p $(BUILD)/cmds
		cp sync $(BUILD)/cmds

sync:		sync.c
		$(BUILD)/bin/lcc -A -o sync sync.c

clean:
		rm -f *~ sync
