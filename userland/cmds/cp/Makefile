#
# Makefile for 'cp' command
#

BUILD = ../../../../build

all:		cp

install:	cp
		mkdir -p $(BUILD)/cmds
		cp cp $(BUILD)/cmds

cp:		cp.c
		$(BUILD)/bin/lcc -A -o cp cp.c

clean:
		rm -f *~ cp
