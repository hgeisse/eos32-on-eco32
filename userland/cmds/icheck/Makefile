#
# Makefile for 'icheck' command
#

BUILD = ../../../build

all:		icheck

install:	icheck
		mkdir -p $(BUILD)/cmds
		cp icheck $(BUILD)/cmds

icheck:		icheck.c
		$(BUILD)/bin/lcc -A -o icheck icheck.c

clean:
		rm -f *~ icheck
