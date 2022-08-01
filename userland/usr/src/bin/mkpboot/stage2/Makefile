#
# Makefile to build the partition bootstrap (boot loader)
#

all:		pboot.bin

pboot.bin:	pboot
		load -p pboot pboot.bin

pboot:		c0.o pboot.o biolib.o
		ld -s stage2.lnk -o pboot \
		  c0.o pboot.o biolib.o

c0.o:		c0.s
		as -o c0.o c0.s

pboot.o:	pboot.c
		lcc -A -c -o pboot.o pboot.c

biolib.o:	biolib.s
		as -o biolib.o biolib.s

clean:
		rm -f *~ c0.o pboot.o biolib.o pboot pboot.bin
