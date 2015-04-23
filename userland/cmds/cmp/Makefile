#
# Makefile for 'cmp' command
#

BUILD = ../../../build

all:		cmp

install:	cmp
		mkdir -p $(BUILD)/cmds
		cp cmp $(BUILD)/cmds

cmp:		cmp.c
		$(BUILD)/bin/lcc -A -o cmp cmp.c

clean:
		rm -f *~ cmp
