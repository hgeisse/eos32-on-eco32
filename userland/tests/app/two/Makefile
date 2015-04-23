#
# Makefile for the 'two' command
#

BUILD = ../../../../build

all:		two

install:	two
		mkdir -p $(BUILD)/tests/app
		cp two $(BUILD)/tests/app

two:		two.c
		$(BUILD)/bin/lcc -A -o two two.c

clean:
		rm -f *~ two
