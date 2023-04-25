#
# Makefile for C math library
#

BUILD = ../../../build

CC = $(BUILD)/bin/lcc
CCFLAGS = -A -I../include/math -I../include

SRCS = acos.c asin.c atan.c atan2.c \
	ceil.c copysign.c cos.c cosh.c \
	exp.c expm1.c \
	floor.c fmod.c frexp.c \
	ldexp.c log.c log10.c \
	modf.c \
	pow.c \
	rem_pio2.c \
	scalbn.c sin.c sinh.c \
	tan.c tanh.c \
	__expo2.c

ASMSRCS = env.s fabs.s sqrt.s

OBJS = $(patsubst %.c,%.o,$(SRCS)) $(patsubst %.s,%.o,$(ASMSRCS))

.PHONY:		all clean

all:		$(OBJS)

%.o:		%.c
		$(BUILD)/bin/lcc $(CCFLAGS) -o $@ -c $<

%.o:		%.s
	$(BUILD)/bin/as -o $@ $<

clean:
		rm -f *~ $(OBJS) $(LIB)
