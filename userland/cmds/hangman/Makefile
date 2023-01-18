#
# Makefile for 'hangman' command
#

BUILD = ../../../build

all:		hangman

install:	hangman
		mkdir -p $(BUILD)/cmds
		cp hangman $(BUILD)/cmds

hangman:	hangman.c
		$(BUILD)/bin/lcc -A -o hangman hangman.c

clean:
		rm -f *~ hangman
