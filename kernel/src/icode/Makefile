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

icode.bin:	icode
		$(BUILD)/bin/load -p icode icode.bin

icode:		icode.o
		$(BUILD)/bin/ld -o icode icode.o

icode.o:	icode.s
		$(BUILD)/bin/as -o icode.o icode.s

clean:
		rm -f *~ dump icode.o icode icode.bin icode.dump
