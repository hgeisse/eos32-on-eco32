#
# Makefile for 'load' command
#

BUILD = ../../../build

all:		load

install:	load
		mkdir -p $(BUILD)/cmds
		cp load $(BUILD)/cmds

load:		load.c
		$(BUILD)/bin/lcc -A -o load load.c

clean:
		rm -f *~ load
