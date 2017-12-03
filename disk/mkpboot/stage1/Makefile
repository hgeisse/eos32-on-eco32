#
# Makefile to build the partition boot record
#

BUILD = ../../../build

all:		pbr.bin

pbr.bin:	pbr
		$(BUILD)/bin/load -p pbr pbr.bin

pbr:		pbr.o
		$(BUILD)/bin/ld -s stage1.lnk -o pbr pbr.o

pbr.o:		pbr.s
		$(BUILD)/bin/as -o pbr.o pbr.s

clean:
		rm -f *~ pbr.o pbr pbr.bin
