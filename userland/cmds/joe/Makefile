#
# Makefile for 'joe' command
#

BUILD = ../../../build

INCS = joe.h buffer.h blocks.h async.h
SRCS = joe.c buffer.c blocks.c cruddy.c

all:		joe

install:	joe
		mkdir -p $(BUILD)/cmds
		cp joe $(BUILD)/cmds

joe:		$(SRCS)
		$(BUILD)/bin/lcc -A -o joe -DKEYDEF=\"x.joerc\" $(SRCS)

clean:
		rm -f *~ joe
