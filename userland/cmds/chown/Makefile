#
# Makefile for 'chown' command
#

BUILD = ../../../../build

all:		chown

install:	chown
		mkdir -p $(BUILD)/cmds
		cp chown $(BUILD)/cmds

chown:		chown.c
		$(BUILD)/bin/lcc -A -o chown chown.c

clean:
		rm -f *~ chown
