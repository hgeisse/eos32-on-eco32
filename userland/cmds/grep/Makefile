#
# Makefile for 'grep' command
#

BUILD = ../../../../build

all:		grep

install:	grep
		mkdir -p $(BUILD)/cmds
		cp grep $(BUILD)/cmds

grep:		grep.c
		$(BUILD)/bin/lcc -A -o grep grep.c

clean:
		rm -f *~ grep
