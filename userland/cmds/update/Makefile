#
# Makefile for 'update' command
#

BUILD = ../../../build

all:		update

install:	update
		mkdir -p $(BUILD)/cmds
		cp update $(BUILD)/cmds

update:		update.c
		$(BUILD)/bin/lcc -A -o update update.c

clean:
		rm -f *~ update
