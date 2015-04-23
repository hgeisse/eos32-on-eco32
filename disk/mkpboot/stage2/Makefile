#
# Makefile to build the partition bootstrap (boot loader)
#

BUILD = ../../../build

all:		pboot.bin

pboot.bin:	c0.o pboot.o biolib.o c1.o
		$(BUILD)/bin/ld -h -rc 0xC0300000 -o pboot.bin \
		  c0.o pboot.o biolib.o c1.o

c0.o:		c0.s
		$(BUILD)/bin/as -o c0.o c0.s

pboot.o:	pboot.c
		$(BUILD)/bin/lcc -A -c -o pboot.o pboot.c

biolib.o:	biolib.s
		$(BUILD)/bin/as -o biolib.o biolib.s

c1.o:		c1.s
		$(BUILD)/bin/as -o c1.o c1.s

clean:
		rm -f *~ c0.o pboot.o biolib.o c1.o pboot.bin
