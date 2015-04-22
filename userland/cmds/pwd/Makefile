#
# Makefile for 'pwd' command
#

BUILD = ../../../../build

all:		pwd

install:	pwd
		mkdir -p $(BUILD)/cmds
		cp pwd $(BUILD)/cmds

pwd:		pwd.c
		$(BUILD)/bin/lcc -A -o pwd pwd.c

clean:
		rm -f *~ pwd
