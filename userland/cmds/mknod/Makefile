#
# Makefile for 'mknod' command
#

BUILD = ../../../../build

all:		mknod

install:	mknod
		mkdir -p $(BUILD)/cmds
		cp mknod $(BUILD)/cmds

mknod:		mknod.c
		$(BUILD)/bin/lcc -A -o mknod mknod.c

clean:
		rm -f *~ mknod
