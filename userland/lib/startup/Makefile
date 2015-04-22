#
# Makefile for building the C startup module
#

BUILD = ../../../../build

all:		c0.o c1.o

c0.o:		c0.s
		$(BUILD)/bin/as -o c0.o c0.s

c1.o:		c1.s
		$(BUILD)/bin/as -o c1.o c1.s

clean:
		rm -f *~ c0.o c1.o
