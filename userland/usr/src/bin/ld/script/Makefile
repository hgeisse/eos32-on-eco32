#
# Makefile for scriptfile parser
#


SRCS = readscript.c utils.c y.tab.c lex.yy.c script.c
OBJS = readscript.o utils.o y.tab.o lex.yy.o script.o
LIB = libreadscript.a

all:		$(LIB)

$(LIB):		$(OBJS)
		ar -cv $(LIB) $(OBJS)

.c.o:
		lcc -A -o $@ -c $<

y.tab.h:	parser.y
		yacc -d parser.y

y.tab.c:	parser.y
		yacc -d parser.y

lex.yy.c:	scanner.l y.tab.h
		lex scanner.l

clean:
		rm -f *~ *.o $(LIB)
		rm -f y.tab.h y.tab.c lex.yy.c
