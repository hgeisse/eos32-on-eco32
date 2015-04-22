#
# Makefile for user mode exception tests
#

BUILD = ../../../../build

SRCS = ux16.s ux17.s ux18.s ux19.s ux22.s ux23.s ux24.s ux25.s
OBJS = $(patsubst %.s,%.o,$(SRCS))
BINS = $(patsubst %.s,%,$(SRCS))

.PHONY:		all install clean

all:		$(BINS)

install:	$(BINS)
		mkdir -p $(BUILD)/tests/ux
		cp $(BINS) $(BUILD)/tests/ux

ux16:		ux16.o
		$(BUILD)/bin/ld -o $@ $<

ux17:		ux17.o
		$(BUILD)/bin/ld -o $@ $<

ux18:		ux18.o
		$(BUILD)/bin/ld -o $@ $<

ux19:		ux19.o
		$(BUILD)/bin/ld -o $@ $<

ux22:		ux22.o
		$(BUILD)/bin/ld -o $@ $<

ux23:		ux23.o
		$(BUILD)/bin/ld -o $@ $<

ux24:		ux24.o
		$(BUILD)/bin/ld -o $@ $<

ux25:		ux25.o
		$(BUILD)/bin/ld -o $@ $<

%.o:		%.s
		$(BUILD)/bin/as -o $@ $<

clean:
		rm -f *~ $(OBJS) $(BINS)
