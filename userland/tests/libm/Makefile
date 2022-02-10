#
# Makefile for EOS32 library tests
#

CC=../../../build/bin/lcc
BUILD=../../../build
#SRCS = expf.c expm1f.c logf.c log10f.c rintf.c \
#			 scalbf.c scalblnf.c scalbnf.c sinf.c cosf.c \
#			 tanf.c sqrtf.c asinf.c fabsf.c acosf.c atanf.c atan2f.c \
#			 sinhf.c coshf.c tanhf.c powf.c
BINS=$(patsubst %.c,%,$(SRCS))

SRCS=sinf.c

INCLUDES=-I$(BUILD)/include/
#INCLUDES=-I../../../lib/include/ -I../../../lib/include/math/ -I../common/ -I../

.PHONY: all install clean

all: $(BINS)

#copysignf.o: copysignf.c
#	$(CC) $(INCLUDES) -o copysignf.o -c copysignf.c

sinf: sinf.c
	$(CC) $(INCLUDES) -o sinf sinf.c -lm

install:	$(BINS)
		mkdir -p $(BUILD)/tests/libm
		cp $(BINS) $(BUILD)/tests/libm

clean:
	rm -f $(BINS) 
