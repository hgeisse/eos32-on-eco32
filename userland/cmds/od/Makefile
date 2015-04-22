#
# Makefile for 'od' command
#

BUILD = ../../../../build

all:		od

install:	od
		mkdir -p $(BUILD)/cmds
		cp od $(BUILD)/cmds

od:		od.c
		$(BUILD)/bin/lcc -A -o od od.c

clean:
		rm -f *~ od
