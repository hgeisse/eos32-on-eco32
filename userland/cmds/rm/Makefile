#
# Makefile for 'rm' command
#

BUILD = ../../../build

all:		rm

install:	rm
		mkdir -p $(BUILD)/cmds
		cp rm $(BUILD)/cmds

rm:		rm.c
		$(BUILD)/bin/lcc -A -o rm rm.c

clean:
		rm -f *~ rm
