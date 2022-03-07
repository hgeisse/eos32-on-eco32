#
# Makefile for 'make' command
#

BUILD = ../../../build

INCS = defs
SRCS = ident.c main.c doname.c misc.c files.c dosys.c gram.tab.c

all:		make

install:	make
		mkdir -p $(BUILD)/cmds
		cp make $(BUILD)/cmds

make:		$(SRCS)
		$(BUILD)/bin/lcc -A -o make $(SRCS)

gram.tab.c:	gram.y
		bison gram.y

clean:
		rm -f *~ gram.tab.c make
