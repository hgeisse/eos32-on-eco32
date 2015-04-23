#
# Makefile for the 'sec2' command
#

BUILD = ../../../../build

all:		sec2

install:	sec2
		mkdir -p $(BUILD)/tests/app
		cp sec2 $(BUILD)/tests/app

sec2:		sec2.c
		$(BUILD)/bin/lcc -A -o sec2 sec2.c

clean:
		rm -f *~ sec2
