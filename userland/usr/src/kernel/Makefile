#
# Makefile for EOS32 kernel
# selfhosting

BUILD = ../build

.PHONY:		all install clean

all:
		make -C src all

install:
		make -C src install
		mkdir -p $(BUILD)/include/sys
		cp include/*.h $(BUILD)/include/sys

clean:
		rm -f include/*~
		make -C src clean
		rm -f *~
