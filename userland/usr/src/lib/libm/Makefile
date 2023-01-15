#
# Makefile for C math library
#

CC = lcc
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

OBJS = acos.o asin.o atan.o atan2.o \
	ceil.o copysign.o cos.o cosh.o \
	exp.o expm1.o \
	floor.o fmod.o frexp.o \
	ldexp.o log.o log10.o \
	modf.o \
	pow.o \
	rem_pio2.o \
	scalbn.o sin.o sinh.o \
	tan.o tanh.o \
	__expo2.o \
	env.o fabs.o sqrt.o

.PHONY:		all clean

all:		$(OBJS)

env.o: env.s
	as -o env.o env.s

fabs.o: fabs.s
	as -o fabs.o fabs.s

sqrt.o: sqrt.s
	as -o sqrt.o sqrt.s

.c.o:
		lcc $(CCFLAGS) -o $@ -c $<

clean:
		rm -f *~ *.o
