#
# Makefile for the 'sig1' command
#

BUILD = ../../../../build

all:		sig1

install:	sig1
		mkdir -p $(BUILD)/tests/app
		cp sig1 $(BUILD)/tests/app

sig1:		sig1.c
		$(BUILD)/bin/lcc -A -o sig1 sig1.c

clean:
		rm -f *~ sig1
