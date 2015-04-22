#
# Makefile for the 'cmdline' command
#

BUILD = ../../../../../build

all:		cmdline

install:	cmdline
		mkdir -p $(BUILD)/tests/app
		cp cmdline $(BUILD)/tests/app

cmdline:	cmdline.c
		$(BUILD)/bin/lcc -A -o cmdline cmdline.c

clean:
		rm -f *~ cmdline
