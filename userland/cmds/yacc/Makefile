#
# Makefile for 'yacc' command
#

BUILD = ../../../build

INCS = files dextern y1.h y2.h y3.h y4.h
SRCS = y1.c y2.c y3.c y4.c

all:		yacc

install:	yacc
		mkdir -p $(BUILD)/cmds
		cp yacc $(BUILD)/cmds
		mkdir -p $(BUILD)/lib
		cp yaccpar $(BUILD)/lib

yacc:		$(INCS) $(SRCS)
		$(BUILD)/bin/lcc -A -o yacc $(SRCS)

clean:
		rm -f *~ yacc
