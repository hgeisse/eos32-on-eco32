#
# Makefile for building EOS32 file systems
#

BUILD = ../..

DSK_IMG = ../disk.img

all:		filesystems

filesystems:
		$(BUILD)/bin/EOS32-mkfs $(DSK_IMG) 0 root.fsys
		$(BUILD)/bin/EOS32-mkfs $(DSK_IMG) 2 usr.fsys
		$(BUILD)/bin/EOS32-mkfs $(DSK_IMG) 3 home.fsys

clean:
		rm -f *~
