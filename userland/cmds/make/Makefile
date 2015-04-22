#
# Makefile for 'make' command
#

BUILD = ../../../../build

INCS = defs
SRCS = ident.c main.c doname.c misc.c files.c dosys.c y.tab.c

all:		make

install:	make
		mkdir -p $(BUILD)/cmds
		cp make $(BUILD)/cmds

make:		$(SRCS)
		$(BUILD)/bin/lcc -A -o make $(SRCS)

y.tab.c:	gram.y
		$(BUILD)/bin/yacc gram.y

clean:
		rm -f *~ y.tab.c make
