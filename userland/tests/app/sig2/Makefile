#
# Makefile for the 'sig2' command
#

BUILD = ../../../../../build

all:		sig2

install:	sig2
		mkdir -p $(BUILD)/tests/app
		cp sig2 $(BUILD)/tests/app

sig2:		sig2.c
		$(BUILD)/bin/lcc -A -o sig2 sig2.c

clean:
		rm -f *~ sig2
