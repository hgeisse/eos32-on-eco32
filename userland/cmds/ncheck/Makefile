#
# Makefile for 'ncheck' command
#

BUILD = ../../../build

all:		ncheck

install:	ncheck
		mkdir -p $(BUILD)/cmds
		cp ncheck $(BUILD)/cmds

ncheck:		ncheck.c
		$(BUILD)/bin/lcc -A -o ncheck ncheck.c

clean:
		rm -f *~ ncheck
