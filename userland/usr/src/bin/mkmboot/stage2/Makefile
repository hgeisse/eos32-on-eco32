#
# Makefile to build the master bootstrap (boot manager)
#


all:		mboot.bin

mboot.bin:	mboot
		load -p mboot mboot.bin

mboot:		c0.o mboot.o biolib.o
		ld -s stage2.lnk -o mboot \
		  c0.o mboot.o biolib.o

c0.o:		c0.s
		as -o c0.o c0.s

mboot.o:	mboot.c
		lcc -A -c -o mboot.o mboot.c

biolib.o:	biolib.s
		as -o biolib.o biolib.s

clean:
		rm -f *~ c0.o mboot.o biolib.o mboot mboot.bin
