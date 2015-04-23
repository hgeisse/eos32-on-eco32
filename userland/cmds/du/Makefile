#
# Makefile for 'du' command
#

BUILD = ../../../build

all:		du

install:	du
		mkdir -p $(BUILD)/cmds
		cp du $(BUILD)/cmds

du:		du.c
		$(BUILD)/bin/lcc -A -o du du.c

clean:
		rm -f *~ du
