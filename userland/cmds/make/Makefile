#
# Makefile for 'make' command
#

BUILD = ../../../build

INCS = defs
SRCS = ident.c main.c doname.c misc.c files.c dosys.c y.tab.c

all:		make

install:	make
		mkdir -p $(BUILD)/cmds
		cp make $(BUILD)/cmds

make:		$(SRCS)
		$(BUILD)/bin/lcc -A -o make $(SRCS)

# original code for YACC
#y.tab.c:	gram.y
#		$(BUILD)/bin/yacc gram.y

# new code for BISON
y.tab.c:	gram-bison.y
		bison --output=y.tab.c gram-bison.y

clean:
		rm -f *~ y.tab.c make
