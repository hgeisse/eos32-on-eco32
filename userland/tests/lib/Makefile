#
# Makefile for EOS32 library tests
#

BUILD = ../../../build

SRCS = assert.c ctype.c eos32.c eos32x.c setjmp.c signal.c \
       stdarg.c stdio.c stdlib.c string.c time.c
BINS = $(patsubst %.c,%,$(SRCS))

.PHONY:		all install clean

all:		$(BINS)

install:	$(BINS)
		mkdir -p $(BUILD)/tests/lib
		cp $(BINS) $(BUILD)/tests/lib

assert:		assert.c
		$(BUILD)/bin/lcc -A -o $@ $<

ctype:		ctype.c
		$(BUILD)/bin/lcc -A -o $@ $<

eos32:		eos32.c
		$(BUILD)/bin/lcc -A -o $@ $<

eos32x:		eos32x.c
		$(BUILD)/bin/lcc -A -o $@ $<

setjmp:		setjmp.c
		$(BUILD)/bin/lcc -A -o $@ $<

signal:		signal.c
		$(BUILD)/bin/lcc -A -o $@ $<

stdarg:		stdarg.c
		$(BUILD)/bin/lcc -A -o $@ $<

stdio:		stdio.c
		$(BUILD)/bin/lcc -A -o $@ $<

stdlib:		stdlib.c
		$(BUILD)/bin/lcc -A -o $@ $<

string:		string.c
		$(BUILD)/bin/lcc -A -o $@ $<

time:		time.c
		$(BUILD)/bin/lcc -A -o $@ $<

clean:
		rm -f *~ $(BINS)
