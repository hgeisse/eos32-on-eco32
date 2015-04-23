#
# Makefile for EOS32 kernel
#

BUILD = ../build

.PHONY:		all install clean

all:
		$(MAKE) -C src all

install:
		$(MAKE) -C src install
		mkdir -p $(BUILD)/include/sys
		cp include/*.h $(BUILD)/include/sys

clean:
		rm -f include/*~
		$(MAKE) -C src clean
		rm -f *~
