#
# Makefile to build the partition boot block
#

BUILD = ../../../build

.PHONY:		all install clean

all:		pbootblk

install:	pbootblk

pbootblk:	stage1/pbr.bin stage2/pboot.bin
		cat stage1/pbr.bin stage2/pboot.bin >pbootblk

stage1/pbr.bin:
		$(MAKE) -C stage1

stage2/pboot.bin:
		$(MAKE) -C stage2

clean:
		$(MAKE) -C stage1 clean
		$(MAKE) -C stage2 clean
		rm -f *~ pbootblk
