#
# Makefile for the 'retval' command
#

BUILD = ../../../../build

all:		retval

install:	retval
		mkdir -p $(BUILD)/tests/app
		cp retval $(BUILD)/tests/app

retval:		retval.c
		$(BUILD)/bin/lcc -A -o retval retval.c

clean:
		rm -f *~ retval
