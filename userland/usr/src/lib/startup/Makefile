#
# Makefile for building the C startup module
#

all:		crt0.o

crt0.o:		crt0.s
		as -o crt0.o crt0.s

clean:
		rm -f *~ crt0.o
