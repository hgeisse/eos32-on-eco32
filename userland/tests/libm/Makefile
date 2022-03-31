#
# Makefile for EOS32 math library tests
#

CC=../../../build/bin/lcc
BUILD=../../../build

SRCS=acos.c asin.c atan.c atan2.c cos.c exp.c expm1.c \
	pow.c sin.c sqrt.c tan.c tanh.c

BINS=$(patsubst %.c,%,$(SRCS))

DIRS=common
INCLUDES=-I$(BUILD)/include/ -I./common/ -I./
LIBS=./common/mtest.o -lm

.PHONY: all dirs install installdirs clean cleandirs

all: dirs $(BINS)

# sin: sin.c
# 	$(CC) $(INCLUDES) -o sin sin.c $(LIBS)
# 
# cos: cos.c
# 	$(CC) $(INCLUDES) -o cos cos.c $(LIBS)

%:	%.c
	$(CC) $(INCLUDES) -o $@ $< $(LIBS)

install:	installdirs $(BINS)
		mkdir -p $(BUILD)/tests/libm
		cp $(BINS) $(BUILD)/tests/libm

dirs:
	for i in $(DIRS) ; do \
		$(MAKE) -C $$i all ; \
	done

installdirs:
	for i in $(DIRS) ; do \
		$(MAKE) -C $$i install ; \
	done

cleandirs:
	for i in $(DIRS) ; do \
		$(MAKE) -C $$i clean ; \
	done

clean: cleandirs
	rm -f $(BINS) 
