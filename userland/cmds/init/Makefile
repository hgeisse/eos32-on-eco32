#
# Makefile for 'init' command
#

BUILD = ../../../build

all:		init

install:	init
		mkdir -p $(BUILD)/cmds
		cp init $(BUILD)/cmds

init:		init.c
		$(BUILD)/bin/lcc -A -o init init.c

clean:
		rm -f *~ init
