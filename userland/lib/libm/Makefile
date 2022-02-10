#
# Makefile for C math library
#

BUILD = ../../../build

CC = $(BUILD)/bin/lcc
CCFLAGS = -A -I../include/math -I../include

SRCS = sinf.c
#SRCS = asfloat.c asuint.c \
#			 copysignf.c \
#			 fabsf.c s_floorf.c scalbnf.c \
#			 e_rem_pio2f.c k_rem_pio2f.c \
#			 k_cosf.c k_sinf.c s_cosf.c s_sinf.c

OBJS = $(patsubst %.c,%.o,$(SRCS))
#LIB = libm.a

.PHONY:		all clean

all:		$(OBJS)

%.o:		%.c
		$(BUILD)/bin/lcc $(CCFLAGS) -o $@ -c $<

clean:
		rm -f *~ $(OBJS) $(LIB)
