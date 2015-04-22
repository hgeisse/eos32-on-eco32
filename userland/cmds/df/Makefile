#
# Makefile for 'df' command
#

BUILD = ../../../../build

all:		df

install:	df
		mkdir -p $(BUILD)/cmds
		cp df $(BUILD)/cmds

df:		df.c
		$(BUILD)/bin/lcc -A -o df df.c

clean:
		rm -f *~ df
