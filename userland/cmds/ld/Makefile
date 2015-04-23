#
# Makefile for 'ld' command
#

BUILD = ../../../build

all:		ld

install:	ld
		mkdir -p $(BUILD)/cmds
		cp ld $(BUILD)/cmds

ld:		ld.c
		$(BUILD)/bin/lcc -A -o ld ld.c

clean:
		rm -f *~ ld
