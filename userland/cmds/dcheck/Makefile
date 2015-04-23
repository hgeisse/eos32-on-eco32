#
# Makefile for 'dcheck' command
#

BUILD = ../../../build

all:		dcheck

install:	dcheck
		mkdir -p $(BUILD)/cmds
		cp dcheck $(BUILD)/cmds

dcheck:		dcheck.c
		$(BUILD)/bin/lcc -A -o dcheck dcheck.c

clean:
		rm -f *~ dcheck
