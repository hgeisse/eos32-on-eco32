#
# Makefile for 'ld' command
#


LIBDIR = script
LIB = libreadscript.a

all:		$(LIBDIR)/$(LIB) ld

$(LIBDIR)/$(LIB):
		cd $(LIBDIR); make all

ld:		ld.c
		lcc -A -I$(LIBDIR) -L$(LIBDIR) \
		  -o ld ld.c -lreadscript

clean:
		cd script; make clean
		rm -f *~ ld
