#
# Makefile for 'sh' command
#

BUILD = ../../../build

INCS = brkincr.h ctyp.h defs.h mac.h mode.h \
       name.h stak.h sym.h timeout.h
SRCS = args.c blok.c builtin.c cmd.c ctyp.c \
       error.c expand.c fault.c io.c macro.c \
       main.c msg.c name.c print.c service.c \
       setbrk.c stak.c string.c word.c xec.c

all:		sh

install:	sh
		mkdir -p $(BUILD)/cmds
		cp sh $(BUILD)/cmds

sh:		$(INCS) $(SRCS)
		$(BUILD)/bin/lcc -A -o sh $(SRCS)

clean:
		rm -f *~ sh
