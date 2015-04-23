#
# Makefile for 'umount' command
#

BUILD = ../../../build

all:		umount

install:	umount
		mkdir -p $(BUILD)/cmds
		cp umount $(BUILD)/cmds

umount:		umount.c
		$(BUILD)/bin/lcc -A -o umount umount.c

clean:
		rm -f *~ umount
