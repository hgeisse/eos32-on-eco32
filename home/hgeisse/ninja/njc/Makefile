#
# Makefile for Ninja Compiler
#

CC = gcc
CFLAGS = -g -Wall -Wno-unused
LDFLAGS = -g
LDLIBS = -lm

SRCS = main.c utils.c parser.tab.c lex.yy.c sym.c \
       absyn.c table.c types.c semant.c codegen.c
OBJS = $(patsubst %.c,%.o,$(SRCS))
BIN = njc

.PHONY:		all tests clean

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

tests:		$(BIN)
		@for i in tests/test??.nj ; do \
		  echo ; \
		  echo -- Compiling $$i -- ; \
		  awk '{if(FNR==2)print}' $$i ; \
		  ./$(BIN) --output tests/`basename $$i .nj`.asm $$i ; \
		done
		@echo

-include depend.mak

depend.mak:	parser.tab.c lex.yy.c
		$(CC) $(CFLAGS) -MM $(SRCS) >depend.mak

clean:
		rm -f *~ *.o $(BIN)
		rm -f parser.tab.h parser.tab.c lex.yy.c depend.mak
		rm -f tests/*~ tests/*.asm
