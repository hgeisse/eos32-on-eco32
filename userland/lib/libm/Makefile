#
# Makefile for C math library
#

BUILD = ../../../build

CC = $(BUILD)/bin/lcc
CCFLAGS = -A -I../include/math -I../include

SRCS = acos.c asin.c \
	copysign.c cos.c pow.c tan.c \
	expm1.c \
	fabs.c floor.c \
	pow.c \
	rem_pio2.c \
	scalbn.c sin.c sqrt.c \
	tan.c tanh.c

OBJS = $(patsubst %.c,%.o,$(SRCS)) env.o

.PHONY:		all clean

all:		$(OBJS)

env.o: env.s
	$(BUILD)/bin/as -o env.o env.s

%.o:		%.c
		$(BUILD)/bin/lcc $(CCFLAGS) -o $@ -c $<

clean:
		rm -f *~ $(OBJS) $(LIB)
