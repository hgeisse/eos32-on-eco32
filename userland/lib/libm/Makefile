#
# Makefile for C math library
#

BUILD = ../../../build

CC = $(BUILD)/bin/lcc
CCFLAGS = -A -I../include/math -I../include

SRCS = sin.c cos.c tan.c pow.c \
	fabs.c floor.c \
	scalbn.c \
	rem_pio2.c copysign.c

OBJS = $(patsubst %.c,%.o,$(SRCS)) env.o

.PHONY:		all clean

all:		$(OBJS)

env.o: env.s
	$(BUILD)/bin/as -o env.o env.s

%.o:		%.c
		$(BUILD)/bin/lcc $(CCFLAGS) -o $@ -c $<

clean:
		rm -f *~ $(OBJS) $(LIB)
