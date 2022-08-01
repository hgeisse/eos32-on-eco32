#
# Makefile to build the partition boot record
#

all:		pbr.bin

pbr.bin:	pbr
		load -p pbr pbr.bin

pbr:		pbr.o
		ld -s stage1.lnk -o pbr pbr.o

pbr.o:		pbr.s
		as -o pbr.o pbr.s

clean:
		rm -f *~ pbr.o pbr pbr.bin
