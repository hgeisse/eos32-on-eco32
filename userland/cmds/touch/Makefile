#
# Makefile for 'touch' command
#

BUILD = ../../../build

all:		touch

install:	touch
		mkdir -p $(BUILD)/cmds
		cp touch $(BUILD)/cmds

touch:		touch.c
		$(BUILD)/bin/lcc -A -o touch touch.c

clean:
		rm -f *~ touch
