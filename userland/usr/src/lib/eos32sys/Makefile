#
# Makefile for building the system call module
#


all:		eos32sys.o

eos32sys.o:	eos32sys.s
		as -o eos32sys.o eos32sys.s

clean:
		rm -f *~ eos32sys.o
