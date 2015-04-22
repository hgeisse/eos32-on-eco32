#
# Makefile for 'cat' command
#

BUILD = ../../../../build

all:		cat

install:	cat
		mkdir -p $(BUILD)/cmds
		cp cat $(BUILD)/cmds

cat:		cat.c
		$(BUILD)/bin/lcc -A -o cat cat.c

clean:
		rm -f *~ cat
