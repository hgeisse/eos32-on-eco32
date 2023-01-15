#
# Makefile for compiling the library modules
#

CC = lcc
CFLAGS = -A -I../include -c

SRCS = assert.c ctype.c signal.c stdio.c stdlib.c \
       string.c time.c errno.c eos32lib.c \
	   stdlib/strtod.c \
	   stdio/printf.c

OBJS = assert.o ctype.o signal.o stdio.o stdlib.o \
       string.o time.o errno.o eos32lib.o \
	   stdlib/strtod.o \
	   stdio/printf.o

.PHONY:		all clean

all:		$(OBJS)
		cp stdlib/*.o .
		cp stdio/*.o .

.c.o:
		$(CC) $(CFLAGS) -o $@ $<

clean:
		rm -f *~ $(OBJS)
