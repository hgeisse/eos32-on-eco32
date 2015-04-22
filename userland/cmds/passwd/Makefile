#
# Makefile for 'passwd' command
#

BUILD = ../../../../build

all:		passwd

install:	passwd
		mkdir -p $(BUILD)/cmds
		cp passwd $(BUILD)/cmds

passwd:		passwd.c
		$(BUILD)/bin/lcc -A -o passwd passwd.c

clean:
		rm -f *~ passwd
