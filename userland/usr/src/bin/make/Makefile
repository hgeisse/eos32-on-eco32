#
# Makefile for 'make' command
# selfhosting

INCS = defs
SRCS = ident.c main.c doname.c misc.c files.c dosys.c gram.tab.c

all:		make

install:	make
		cp make /bin

make:		$(SRCS)
		lcc -A -o make $(SRCS)

gram.tab.c:	gram.y
		yacc gram.y
		mv y.tab.c gram.tab.c

clean:
		rm -f *~ gram.tab.c make
