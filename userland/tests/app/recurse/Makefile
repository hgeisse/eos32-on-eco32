#
# Makefile for the 'recurse' command
#

BUILD = ../../../../../build

all:		recurse

install:	recurse
		mkdir -p $(BUILD)/tests/app
		cp recurse $(BUILD)/tests/app

recurse:	recurse.c
		$(BUILD)/bin/lcc -A -o recurse recurse.c

clean:
		rm -f *~ recurse
