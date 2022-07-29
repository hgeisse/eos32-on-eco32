#
# Makefile to build the master boot block (with an empty partition table)
#


.PHONY:		all install clean

all:		mbootblk

install:	mbootblk

mbootblk:	stage1/mbr.bin stage2/mboot.bin
		dd if=/dev/zero of=./zeroes bs=512 count=1
		cat stage1/mbr.bin zeroes stage2/mboot.bin >mbootblk

stage1/mbr.bin:
		cd stage1
		$(MAKE)

stage2/mboot.bin:
		cd stage2
		$(MAKE)

clean:	
		cd stage1
		$(MAKE) clean
		cd ..
		cd stage2		
		$(MAKE) clean
		cd ..
		rm -f *~ zeroes mbootblk
