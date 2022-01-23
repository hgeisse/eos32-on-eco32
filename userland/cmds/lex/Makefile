#
# Makefile for 'lex' command
#

BUILD = ../../../build

all:		lex

install:	lex
		mkdir -p $(BUILD)/cmds
		cp lex $(BUILD)/cmds

lex:		lex.c
		$(BUILD)/bin/lcc -A -o lex lex.c

clean:
		rm -f *~ lex
