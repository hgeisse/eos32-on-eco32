#
# Makefile for 'mkfs' command
#

BUILD = ../../../build

all:		mkfs

install:	mkfs
		mkdir -p $(BUILD)/cmds
		cp mkfs $(BUILD)/cmds

mkfs:		mkfs.c
		$(BUILD)/bin/lcc -A -o mkfs mkfs.c

clean:
		rm -f *~ mkfs
