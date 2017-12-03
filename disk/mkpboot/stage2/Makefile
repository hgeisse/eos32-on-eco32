#
# Makefile to build the partition bootstrap (boot loader)
#

BUILD = ../../../build

all:		pboot.bin

pboot.bin:	pboot
		$(BUILD)/bin/load -p pboot pboot.bin

pboot:		c0.o pboot.o biolib.o
		$(BUILD)/bin/ld -s stage2.lnk -o pboot \
		  c0.o pboot.o biolib.o

c0.o:		c0.s
		$(BUILD)/bin/as -o c0.o c0.s

pboot.o:	pboot.c
		$(BUILD)/bin/lcc -A -c -o pboot.o pboot.c

biolib.o:	biolib.s
		$(BUILD)/bin/as -o biolib.o biolib.s

clean:
		rm -f *~ c0.o pboot.o biolib.o pboot pboot.bin
