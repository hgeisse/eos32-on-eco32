#
# Makefile for building the setjmp module
#

all:		setjmp.o

setjmp.o:	setjmp.s
		as -o setjmp.o setjmp.s

clean:
		rm -f *~ setjmp.o
