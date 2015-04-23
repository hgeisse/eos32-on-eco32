#
# Makefile for the 'sec1' command
#

BUILD = ../../../../build

all:		sec1

install:	sec1
		mkdir -p $(BUILD)/tests/app
		cp sec1 $(BUILD)/tests/app

sec1:		sec1.c
		$(BUILD)/bin/lcc -A -o sec1 sec1.c

clean:
		rm -f *~ sec1
