#
# Makefile for 'echo' command
#

BUILD = ../../../../build

all:		echo

install:	echo
		mkdir -p $(BUILD)/cmds
		cp echo $(BUILD)/cmds

echo:		echo.c
		$(BUILD)/bin/lcc -A -o echo echo.c

clean:
		rm -f *~ echo
