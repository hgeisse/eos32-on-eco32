#
# Makefile for 'clri' command
#

BUILD = ../../../../build

all:		clri

install:	clri
		mkdir -p $(BUILD)/cmds
		cp clri $(BUILD)/cmds

clri:		clri.c
		$(BUILD)/bin/lcc -A -o clri clri.c

clean:
		rm -f *~ clri
