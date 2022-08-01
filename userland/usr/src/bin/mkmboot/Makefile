#
# Makefile to build the master boot block (with an empty partition table)
#

.PHONY:		all install clean

all:		mbootblk

install:	mbootblk
		mkdir -p $(BUILD)/run
		cp mbootblk $(BUILD)/run

mbootblk:	stage1/mbr.bin stage2/mboot.bin
		dd if=/dev/zero of=./zeroes bs=512 count=1
		cat stage1/mbr.bin zeroes stage2/mboot.bin >mbootblk

stage1/mbr.bin:
		cd stage1; make
#		$(MAKE) -C stage1

stage2/mboot.bin:
		cd stage2; make
#		$(MAKE) -C stage2

clean:
		cd stage1; make clean
		cd stage2; make clean
#		$(MAKE) -C stage1 clean
#		$(MAKE) -C stage2 clean
		rm -f *~ zeroes mbootblk
