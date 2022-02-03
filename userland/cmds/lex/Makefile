#
# Makefile for 'lex' command
#

BUILD = ../../../build

INCS = ldefs.c once.c lmain.h sub1.h sub2.h header.h parser.h
SRCS = lmain.c sub1.c sub2.c header.c parser.tab.c

all:		lex

install:	lex
		mkdir -p $(BUILD)/cmds
		cp lex $(BUILD)/cmds
		mkdir -p $(BUILD)/lib
		cp ncform $(BUILD)/lib

lex:		$(INCS) $(SRCS)
		$(BUILD)/bin/lcc -A -o lex $(SRCS)

parser.tab.c:	parser.y
		bison parser.y

clean:
		rm -f *~ parser.tab.c lex
