#
# Makefile for 'mkdir' command
#

BUILD = ../../../build

all:		mkdir

install:	mkdir
		mkdir -p $(BUILD)/cmds
		cp mkdir $(BUILD)/cmds

mkdir:		mkdir.c
		$(BUILD)/bin/lcc -A -o mkdir mkdir.c

clean:
		rm -f *~ mkdir
