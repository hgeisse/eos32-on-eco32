#
# Makefile for 'dd' command
#

BUILD = ../../../build

all:		dd

install:	dd
		mkdir -p $(BUILD)/cmds
		cp dd $(BUILD)/cmds

dd:		dd.c
		$(BUILD)/bin/lcc -A -o dd dd.c

clean:
		rm -f *~ dd
