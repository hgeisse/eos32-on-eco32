#
# Makefile for EOS32 disk tools
#

BUILD = ../build
TARGET = $(BUILD)/run/fs-EOS32

DIRS = mkfstest mkdata mkpboot mkfs shfs

.PHONY:		all install clean

all:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i all ; \
		done

install:
		mkdir -p $(TARGET)
		rm -rf $(TARGET)/*
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i install ; \
		done
		cp Makefile.run $(TARGET)/Makefile
		cp -R config/* $(TARGET)

clean:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i clean ; \
		done
		rm -f *~
