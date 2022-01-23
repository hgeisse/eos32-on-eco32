#
# Makefile for EOS32-on-ECO32 project
#

VERSION = 0.1

DIRS = kernel userland disk
BUILD = ./build

all:		clean-dirs
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i install ; \
		done

clean-dirs:	build-link
		rm -rf $(BUILD)/include/*
		rm -rf $(BUILD)/lib/*

build-link:
		./make-link $(BUILD)

mostly-clean:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i clean ; \
		done
		rm -f *~

clean:		mostly-clean
		rm -f build
