#
# Makefile for 'pr' command
#

BUILD = ../../../build

all:		pr

install:	pr
		mkdir -p $(BUILD)/cmds
		cp pr $(BUILD)/cmds

pr:		pr.c
		$(BUILD)/bin/lcc -A -o pr pr.c

clean:
		rm -f *~ pr
