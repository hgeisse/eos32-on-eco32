#
# Makefile to build the master boot record
#

BUILD = ../../../../build

all:		mbr.bin

mbr.bin:	mbr
		load -p mbr mbr.bin

mbr:		mbr.o
		ld -s stage1.lnk -o mbr mbr.o

mbr.o:		mbr.s
		as -o mbr.o mbr.s

clean:
		rm -f *~ mbr.o mbr mbr.bin
