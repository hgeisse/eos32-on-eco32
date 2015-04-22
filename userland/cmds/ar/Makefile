#
# Makefile for 'ar' command
#

BUILD = ../../../../build

all:		ar

install:	ar
		mkdir -p $(BUILD)/cmds
		cp ar $(BUILD)/cmds

ar:		ar.c
		$(BUILD)/bin/lcc -A -o ar ar.c

clean:
		rm -f *~ ar
