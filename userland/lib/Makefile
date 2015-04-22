#
# Makefile for EOS32 standard library
#

BUILD = ../../../build

.PHONY:		all install compile clean

all:		compile

install:	compile
		mkdir -p $(BUILD)/include
		cp include/*.h $(BUILD)/include
		mkdir -p $(BUILD)/lib
		cp startup/*.o $(BUILD)/lib
		cp setjmp/*.o $(BUILD)/lib
		cp eos32sys/*.o $(BUILD)/lib
		cp libsrc/*.o $(BUILD)/lib

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
		rm -f *~
