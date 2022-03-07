#
# Makefile for 'as' command
#

BUILD = ../../../build

all:		as

install:	as
		mkdir -p $(BUILD)/cmds
		cp as $(BUILD)/cmds

as:		as.c
		$(BUILD)/bin/lcc -A -o as as.c -lm

clean:
		rm -f *~ as
