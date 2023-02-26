#
# Makefile for 'deroff' command
#

BUILD = ../../../build

all:		deroff

install:	deroff
		mkdir -p $(BUILD)/cmds
		cp deroff $(BUILD)/cmds

deroff:		deroff.c
		$(BUILD)/bin/lcc -A -o deroff deroff.c

clean:
		rm -f *~ deroff
