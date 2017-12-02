#
# Makefile for building the C startup module
#

BUILD = ../../../build

all:		crt0.o

crt0.o:		crt0.s
		$(BUILD)/bin/as -o crt0.o crt0.s

clean:
		rm -f *~ crt0.o
