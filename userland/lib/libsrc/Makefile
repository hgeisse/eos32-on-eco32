#
# Makefile for compiling the library modules
#

BUILD = ../../../build

CC = $(BUILD)/bin/lcc
CFLAGS = -A -I../include -c

SRCS = assert.c ctype.c math.c signal.c stdio.c stdlib.c \
       string.c time.c errno.c eos32lib.c \
	   math/log.c

OBJS = $(patsubst %.c,%.o,$(SRCS))

.PHONY:		all clean

all:		$(OBJS)

%.o:		%.c
		$(CC) $(CFLAGS) -o $@ $<

clean:
		rm -f *~ $(OBJS)
