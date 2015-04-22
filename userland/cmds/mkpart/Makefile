#
# Makefile for 'mkpart' command
#

BUILD = ../../../../build

all:		mkpart

install:	mkpart
		mkdir -p $(BUILD)/cmds
		cp mkpart $(BUILD)/cmds

mkpart:		mkpart.c
		$(BUILD)/bin/lcc -A -o mkpart mkpart.c

clean:
		rm -f *~ mkpart
