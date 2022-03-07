#
# Makefile for Ninja Assembler
#

CC = gcc
CFLAGS = -g -Wall -Wno-unused
LDFLAGS = -g
LDLIBS = -lm

SRCS = main.c utils.c parser.tab.c lex.yy.c coder.c table.c
OBJS = $(patsubst %.c,%.o,$(SRCS))
BIN = nja

.PHONY:		all clean

all:		$(BIN)

$(BIN):		$(OBJS)
		$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o:		%.c
		$(CC) $(CFLAGS) -o $@ -c $<

parser.tab.h:	parser.y
		bison -d parser.y

parser.tab.c:	parser.y
		bison -d parser.y

lex.yy.c:	scanner.l parser.tab.h
		flex scanner.l

-include depend.mak

depend.mak:	parser.tab.c lex.yy.c
		$(CC) $(CFLAGS) -MM $(SRCS) >depend.mak

clean:
		rm -f *~ *.o $(BIN)
		rm -f parser.tab.h parser.tab.c lex.yy.c depend.mak
