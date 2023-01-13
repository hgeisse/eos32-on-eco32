#
# Makefile for 'joe' command
#

INCS = joe.h buffer.h blocks.h async.h
SRCS = joe.c buffer.c blocks.c cruddy.c

all:		joe

joe:		$(SRCS)
		lcc -A -o joe -DKEYDEF=\"/etc/joerc\" $(SRCS)

clean:
		rm -f *~ joe
