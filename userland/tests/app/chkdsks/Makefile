#
# Makefile for the 'chkdsks' command
#

BUILD = ../../../../build

all:		chkdsks

install:	chkdsks
		mkdir -p $(BUILD)/tests/app
		cp chkdsks $(BUILD)/tests/app

chkdsks:	chkdsks.c
		$(BUILD)/bin/lcc -A -o chkdsks chkdsks.c

clean:
		rm -f *~ chkdsks
