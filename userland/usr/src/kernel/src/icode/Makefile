#
# Makefile for icode, the initial code for process 1
#

.PHONY:		all clean

all:		icode.dump

icode.dump:	dump icode.bin
		./dump icode.bin icode.dump

dump:		dump.c
		lcc -A -o dump dump.c

icode.bin:	icode
		load -p icode icode.bin

icode:		icode.o
		ld -o icode icode.o

icode.o:	icode.s
		as -o icode.o icode.s

clean:
		rm -f *~ dump icode.o icode icode.bin icode.dump
