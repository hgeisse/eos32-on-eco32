#
# Makefile for 'tail' command
# 


all:		tail

install:	tail
		cp tail /bin/tail

tail:		tail.c
		lcc -A -o tail tail.c

clean:
		rm -f *~ tail
