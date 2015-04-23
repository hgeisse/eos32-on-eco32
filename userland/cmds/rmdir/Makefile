#
# Makefile for 'rmdir' command
#

BUILD = ../../../build

all:		rmdir

install:	rmdir
		mkdir -p $(BUILD)/cmds
		cp rmdir $(BUILD)/cmds

rmdir:		rmdir.c
		$(BUILD)/bin/lcc -A -o rmdir rmdir.c

clean:
		rm -f *~ rmdir
