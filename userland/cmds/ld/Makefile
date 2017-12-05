#
# Makefile for 'ld' command
#

BUILD = ../../../build

LIBDIR = script
LIB = libreadscript.a

all:		$(LIBDIR)/$(LIB) ld

$(LIBDIR)/$(LIB):
		$(MAKE) -C $(LIBDIR) all

install:	all
		mkdir -p $(BUILD)/cmds
		cp ld $(BUILD)/cmds

ld:		ld.c
		$(BUILD)/bin/lcc -A -I$(LIBDIR) -L$(LIBDIR) \
		  -o ld ld.c -lreadscript

clean:
		$(MAKE) -C script clean
		rm -f *~ ld
