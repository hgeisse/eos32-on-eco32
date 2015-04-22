#
# Makefile for 'shpart' command
#

BUILD = ../../../../build

all:		shpart

install:	shpart
		mkdir -p $(BUILD)/cmds
		cp shpart $(BUILD)/cmds

shpart:		shpart.c
		$(BUILD)/bin/lcc -A -o shpart shpart.c

clean:
		rm -f *~ shpart
