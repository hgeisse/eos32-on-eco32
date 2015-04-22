#
# Makefile for 'mount' command
#

BUILD = ../../../../build

all:		mount

install:	mount
		mkdir -p $(BUILD)/cmds
		cp mount $(BUILD)/cmds

mount:		mount.c
		$(BUILD)/bin/lcc -A -o mount mount.c

clean:
		rm -f *~ mount
