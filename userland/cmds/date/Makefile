#
# Makefile for 'date' command
#

BUILD = ../../../../build

all:		date

install:	date
		mkdir -p $(BUILD)/cmds
		cp date $(BUILD)/cmds

date:		date.c
		$(BUILD)/bin/lcc -A -o date date.c

clean:
		rm -f *~ date
