#
# Makefile for building the setjmp module
#

BUILD = ../../../build

all:		setjmp.o

setjmp.o:	setjmp.s
		$(BUILD)/bin/as -o setjmp.o setjmp.s

clean:
		rm -f *~ setjmp.o
