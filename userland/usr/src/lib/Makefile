#
# Makefile for EOS32 standard library
# 

.PHONY:		all install archive compile clean

all:		archive

archive:	compile
		ar -cv libc.a \
		  setjmp/*.o eos32sys/*.o libsrc/*.o
#		ar -cv libm.a libm/*.o

compile:
		cd startup; make all
		cd setjmp; make all
		cd eos32sys; make all
		cd libsrc; make all
#		cd libm; make all

clean:
		rm -f include/*~
		cd startup; make clean
		cd setjmp; make clean
		cd eos32sys; make clean
		cd libsrc; make clean
#		cd libm; make clean
		rm -f *~ libc.a libm.a
