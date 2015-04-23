#
# Makefile for icode, the initial code for process 1
#

BUILD = ../../../build

.PHONY:		all clean

all:		icode.dump

icode.dump:	dump icode.bin
		./dump icode.bin icode.dump

dump:		dump.c
		gcc -Wall -o dump dump.c

icode.bin:	icode.o
		$(BUILD)/bin/ld -h -o icode.bin icode.o

icode.o:	icode.s
		$(BUILD)/bin/as -o icode.o icode.s

clean:
		rm -f *~ dump icode.o icode.bin icode.dump
