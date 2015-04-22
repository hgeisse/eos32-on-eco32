#
# Makefile for EOS32 init
#

BUILD = ../../../../../build

all:		init

install:	init
		mkdir -p $(BUILD)/tests/app
		cp init $(BUILD)/tests/app

init:		init.c
		$(BUILD)/bin/lcc -A -o init init.c

clean:
		rm -f *~ init
