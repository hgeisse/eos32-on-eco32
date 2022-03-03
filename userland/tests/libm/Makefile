#
# Makefile for EOS32 math library tests
#

CC=../../../build/bin/lcc
BUILD=../../../build

SRCS=sin.c foo.c

BINS=$(patsubst %.c,%,$(SRCS))

DIRS=common
INCLUDES=-I$(BUILD)/include/ -I./common/ -I./

.PHONY: all dirs install installdirs clean cleandirs

all: dirs $(BINS)

sin: sin.c
	$(CC) $(INCLUDES) -o sin sin.c ./common/mtest.o -lm

install:	installdirs $(BINS)
		mkdir -p $(BUILD)/tests/libm
		cp $(BINS) $(BUILD)/tests/libm

dirs:
	for i in $(DIRS) ; do \
		$(MAKE) -C $$i all ; \
	done

installdirs:
	for i in $(DIRS) ; do \
		$(MAKE) -C $$i install ; \
	done

cleandirs:
	for i in $(DIRS) ; do \
		$(MAKE) -C $$i clean ; \
	done

clean: cleandirs
	rm -f $(BINS) 
