#
# Makefile for the 'argcopy' command
#

BUILD = ../../../../build

all:		argcopy

install:	argcopy
		mkdir -p $(BUILD)/tests/app
		cp argcopy $(BUILD)/tests/app

argcopy:	argcopy.c
		$(BUILD)/bin/lcc -A -o argcopy argcopy.c

clean:
		rm -f *~ argcopy
