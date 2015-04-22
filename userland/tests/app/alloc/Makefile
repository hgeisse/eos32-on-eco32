#
# Makefile for the 'alloc' command
#

BUILD = ../../../../../build

all:		alloc

install:	alloc
		mkdir -p $(BUILD)/tests/app
		cp alloc $(BUILD)/tests/app

alloc:		alloc.c
		$(BUILD)/bin/lcc -A -o alloc alloc.c

clean:
		rm -f *~ alloc
