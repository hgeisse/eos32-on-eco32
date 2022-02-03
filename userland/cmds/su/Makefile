#
# Makefile for 'su' command
#

BUILD = ../../../build

all:		su

install:	su
		mkdir -p $(BUILD)/cmds
		cp su $(BUILD)/cmds

su:		su.c
		$(BUILD)/bin/lcc -A -o su su.c

clean:
		rm -f *~ su
