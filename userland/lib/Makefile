#
# Makefile for EOS32 standard library
#

BUILD = ../../build

.PHONY:		all install archive compile clean

all:		archive

install:	archive
		mkdir -p $(BUILD)/include
		cp include/*.h $(BUILD)/include
		mkdir -p $(BUILD)/lib
		cp scripts/*.lnk $(BUILD)/lib
		cp startup/*.o $(BUILD)/lib
		cp libc.a $(BUILD)/lib

archive:	compile
		$(BUILD)/bin/ar -cv libc.a \
		  setjmp/*.o eos32sys/*.o libsrc/*.o

compile:
		$(MAKE) -C startup all
		$(MAKE) -C setjmp all
		$(MAKE) -C eos32sys all
		$(MAKE) -C libsrc all

clean:
		rm -f include/*~
		$(MAKE) -C startup clean
		$(MAKE) -C setjmp clean
		$(MAKE) -C eos32sys clean
		$(MAKE) -C libsrc clean
		rm -f *~ libc.a
