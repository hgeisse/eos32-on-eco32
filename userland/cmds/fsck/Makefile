#
# Makefile for 'fsck' command
#

BUILD = ../../../build

all:		fsck

install:	fsck
		mkdir -p $(BUILD)/cmds
		cp fsck $(BUILD)/cmds

fsck:		fsck.c
		$(BUILD)/bin/lcc -A -o fsck fsck.c

clean:
		rm -f *~ fsck
