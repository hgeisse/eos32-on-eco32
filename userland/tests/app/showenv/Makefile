#
# Makefile for the 'showenv' command
#

BUILD = ../../../../../build

all:		showenv

install:	showenv
		mkdir -p $(BUILD)/tests/app
		cp showenv $(BUILD)/tests/app

showenv:	showenv.c
		$(BUILD)/bin/lcc -A -o showenv showenv.c

clean:
		rm -f *~ showenv
