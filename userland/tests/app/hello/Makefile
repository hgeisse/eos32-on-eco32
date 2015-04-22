#
# Makefile for the 'hello' command
#

BUILD = ../../../../../build

all:		hello

install:	hello
		mkdir -p $(BUILD)/tests/app
		cp hello $(BUILD)/tests/app

hello:		hello.c
		$(BUILD)/bin/lcc -A -o hello hello.c

clean:
		rm -f *~ hello
