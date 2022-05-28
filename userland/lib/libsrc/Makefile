#
# Makefile for compiling the library modules
#

BUILD = ../../../build

CC = $(BUILD)/bin/lcc
CFLAGS = -A -I../include -c

SRCS = assert.c ctype.c signal.c stdio.c stdlib.c \
       string.c time.c errno.c eos32lib.c \
	   stdlib/strtod.c \
	   stdio/printf.c

OBJS = $(patsubst %.c,%.o,$(SRCS))

.PHONY:		all clean

all:		$(OBJS)
		cp -r stdlib/*.o .
		cp -r stdio/*.o .

%.o:		%.c
		$(CC) $(CFLAGS) -o $@ $<

clean:
		rm -f *~ $(OBJS)
