#
# Makefile for scriptfile parser
#

BUILD = ../../../../build

SRCS = readscript.c utils.c parser.tab.c lex.yy.c script.c
OBJS = $(patsubst %.c,%.o,$(SRCS))
LIB = libreadscript.a

all:		$(LIB)

$(LIB):		$(OBJS)
		$(BUILD)/bin/ar -cv $(LIB) $(OBJS)

%.o:		%.c
		$(BUILD)/bin/lcc -A -o $@ -c $<

parser.tab.h:	parser.y
		bison -d parser.y

parser.tab.c:	parser.y
		bison -d parser.y

lex.yy.c:	scanner.l parser.tab.h
		flex scanner.l

clean:
		rm -f *~ *.o $(LIB)
		rm -f parser.tab.h parser.tab.c lex.yy.c
