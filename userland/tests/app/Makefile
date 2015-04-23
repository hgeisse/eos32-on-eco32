#
# Makefile for EOS32 userland tests
#

BUILD = ../../../build

DIRS = init hello primes cmdline showenv retval db sec1 \
       sec2 two sig1 sig2 recurse chkdsks argcopy alloc

.PHONY:		all install clean

all:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i all ; \
		done

install:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i install ; \
		done

clean:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i clean ; \
		done
		rm -f *~
