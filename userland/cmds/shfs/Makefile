#
# Makefile for 'shfs' command
#

BUILD = ../../../../build

all:		shfs

install:	shfs
		mkdir -p $(BUILD)/cmds
		cp shfs $(BUILD)/cmds

shfs:		shfs.c
		$(BUILD)/bin/lcc -A -o shfs shfs.c

clean:
		rm -f *~ shfs
