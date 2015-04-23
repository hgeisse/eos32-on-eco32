#
# Makefile for 'wc' command
#

BUILD = ../../../build

all:		wc

install:	wc
		mkdir -p $(BUILD)/cmds
		cp wc $(BUILD)/cmds

wc:		wc.c
		$(BUILD)/bin/lcc -A -o wc wc.c

clean:
		rm -f *~ wc
