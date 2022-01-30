#
# Makefile for 'cat' command
#

BUILD = ../../../build

all:		tail

install:	tail
		mkdir -p $(BUILD)/cmds
		cp tail $(BUILD)/cmds

tail:		tail.c
		$(BUILD)/bin/lcc -A -o tail tail.c

clean:
		rm -f *~ tail
