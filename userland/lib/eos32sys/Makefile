#
# Makefile for building the system call module
#

BUILD = ../../../build

all:		eos32sys.o

eos32sys.o:	eos32sys.s
		$(BUILD)/bin/as -o eos32sys.o eos32sys.s

clean:
		rm -f *~ eos32sys.o
