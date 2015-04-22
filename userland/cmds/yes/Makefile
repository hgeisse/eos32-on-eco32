#
# Makefile for 'yes' command
#

BUILD = ../../../../build

all:		yes

install:	yes
		mkdir -p $(BUILD)/cmds
		cp yes $(BUILD)/cmds

yes:		yes.c
		$(BUILD)/bin/lcc -A -o yes yes.c

clean:
		rm -f *~ yes
