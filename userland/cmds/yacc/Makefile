#
# Makefile for 'yacc' command
#

BUILD = ../../../build

all:		yacc

install:	yacc
		mkdir -p $(BUILD)/cmds
		cp yacc $(BUILD)/cmds

yacc:		yacc.c
		$(BUILD)/bin/lcc -A -o yacc yacc.c

clean:
		rm -f *~ yacc
