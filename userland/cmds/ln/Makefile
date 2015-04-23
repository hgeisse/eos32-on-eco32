#
# Makefile for 'ln' command
#

BUILD = ../../../build

all:		ln

install:	ln
		mkdir -p $(BUILD)/cmds
		cp ln $(BUILD)/cmds

ln:		ln.c
		$(BUILD)/bin/lcc -A -o ln ln.c

clean:
		rm -f *~ ln
