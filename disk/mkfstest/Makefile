#
# Makefile to generate files for file system test
#

BUILD = ../../build

CC = gcc
CFLAGS = -g -Wall

.PHONY:		all install clean

all:		1KiB

install:	1KiB
		mkdir -p $(BUILD)/fstest
		cp *KiB $(BUILD)/fstest
		cp check $(BUILD)/fstest
		cp README $(BUILD)/fstest

1KiB:		mkfstest
		./mkfstest --random

mkfstest:	mkfstest.c
		$(CC) $(CFLAGS) -o mkfstest mkfstest.c

clean:
		rm -f *~ mkfstest *KiB check
