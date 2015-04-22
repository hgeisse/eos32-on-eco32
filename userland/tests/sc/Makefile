#
# Makefile for syscall tests
#

BUILD = ../../../../build

SRCS = sc00.s sc01.s sc02.s sc03.s sc04.s sc05.s sc06.s sc07.s \
       sc08.s sc09.s sc10.s sc11.s sc12.s sc13.s sc14.s sc15.s \
       sc16.s sc17.s sc18.s sc19.s sc20.s sc21.s sc22.s sc23.s \
       sc24.s sc25.s sc26.s sc27.s sc28.s sc29.s sc30.s sc31.s \
       sc32.s sc33.s sc34.s sc35.s sc36.s sc37.s sc38.s sc39.s \
       sc40.s sc41.s sc42.s sc43.s sc44.s sc45.s sc46.s sc47.s \
       sc48.s sc49.s sc50.s sc51.s sc52.s sc53.s sc54.s sc55.s \
       sc56.s sc57.s sc58.s sc59.s sc59x.s sc60.s sc61.s sc62.s sc63.s
OBJS = $(patsubst %.s,%.o,$(SRCS))
BINS = $(patsubst %.s,%,$(SRCS))

.PHONY:		all install clean

all:		$(BINS)

install:	$(BINS)
		mkdir -p $(BUILD)/tests/sc
		cp $(BINS) $(BUILD)/tests/sc

sc00:		sc00.o
		$(BUILD)/bin/ld -o $@ $<

sc01:		sc01.o
		$(BUILD)/bin/ld -o $@ $<

sc02:		sc02.o
		$(BUILD)/bin/ld -o $@ $<

sc03:		sc03.o
		$(BUILD)/bin/ld -o $@ $<

sc04:		sc04.o
		$(BUILD)/bin/ld -o $@ $<

sc05:		sc05.o
		$(BUILD)/bin/ld -o $@ $<

sc06:		sc06.o
		$(BUILD)/bin/ld -o $@ $<

sc07:		sc07.o
		$(BUILD)/bin/ld -o $@ $<

sc08:		sc08.o
		$(BUILD)/bin/ld -o $@ $<

sc09:		sc09.o
		$(BUILD)/bin/ld -o $@ $<

sc10:		sc10.o
		$(BUILD)/bin/ld -o $@ $<

sc11:		sc11.o
		$(BUILD)/bin/ld -o $@ $<

sc12:		sc12.o
		$(BUILD)/bin/ld -o $@ $<

sc13:		sc13.o
		$(BUILD)/bin/ld -o $@ $<

sc14:		sc14.o
		$(BUILD)/bin/ld -o $@ $<

sc15:		sc15.o
		$(BUILD)/bin/ld -o $@ $<

sc16:		sc16.o
		$(BUILD)/bin/ld -o $@ $<

sc17:		sc17.o
		$(BUILD)/bin/ld -o $@ $<

sc18:		sc18.o
		$(BUILD)/bin/ld -o $@ $<

sc19:		sc19.o
		$(BUILD)/bin/ld -o $@ $<

sc20:		sc20.o
		$(BUILD)/bin/ld -o $@ $<

sc21:		sc21.o
		$(BUILD)/bin/ld -o $@ $<

sc22:		sc22.o
		$(BUILD)/bin/ld -o $@ $<

sc23:		sc23.o
		$(BUILD)/bin/ld -o $@ $<

sc24:		sc24.o
		$(BUILD)/bin/ld -o $@ $<

sc25:		sc25.o
		$(BUILD)/bin/ld -o $@ $<

sc26:		sc26.o
		$(BUILD)/bin/ld -o $@ $<

sc27:		sc27.o
		$(BUILD)/bin/ld -o $@ $<

sc28:		sc28.o
		$(BUILD)/bin/ld -o $@ $<

sc29:		sc29.o
		$(BUILD)/bin/ld -o $@ $<

sc30:		sc30.o
		$(BUILD)/bin/ld -o $@ $<

sc31:		sc31.o
		$(BUILD)/bin/ld -o $@ $<

sc32:		sc32.o
		$(BUILD)/bin/ld -o $@ $<

sc33:		sc33.o
		$(BUILD)/bin/ld -o $@ $<

sc34:		sc34.o
		$(BUILD)/bin/ld -o $@ $<

sc35:		sc35.o
		$(BUILD)/bin/ld -o $@ $<

sc36:		sc36.o
		$(BUILD)/bin/ld -o $@ $<

sc37:		sc37.o
		$(BUILD)/bin/ld -o $@ $<

sc38:		sc38.o
		$(BUILD)/bin/ld -o $@ $<

sc39:		sc39.o
		$(BUILD)/bin/ld -o $@ $<

sc40:		sc40.o
		$(BUILD)/bin/ld -o $@ $<

sc41:		sc41.o
		$(BUILD)/bin/ld -o $@ $<

sc42:		sc42.o
		$(BUILD)/bin/ld -o $@ $<

sc43:		sc43.o
		$(BUILD)/bin/ld -o $@ $<

sc44:		sc44.o
		$(BUILD)/bin/ld -o $@ $<

sc45:		sc45.o
		$(BUILD)/bin/ld -o $@ $<

sc46:		sc46.o
		$(BUILD)/bin/ld -o $@ $<

sc47:		sc47.o
		$(BUILD)/bin/ld -o $@ $<

sc48:		sc48.o
		$(BUILD)/bin/ld -o $@ $<

sc49:		sc49.o
		$(BUILD)/bin/ld -o $@ $<

sc50:		sc50.o
		$(BUILD)/bin/ld -o $@ $<

sc51:		sc51.o
		$(BUILD)/bin/ld -o $@ $<

sc52:		sc52.o
		$(BUILD)/bin/ld -o $@ $<

sc53:		sc53.o
		$(BUILD)/bin/ld -o $@ $<

sc54:		sc54.o
		$(BUILD)/bin/ld -o $@ $<

sc55:		sc55.o
		$(BUILD)/bin/ld -o $@ $<

sc56:		sc56.o
		$(BUILD)/bin/ld -o $@ $<

sc57:		sc57.o
		$(BUILD)/bin/ld -o $@ $<

sc58:		sc58.o
		$(BUILD)/bin/ld -o $@ $<

sc59:		sc59.o
		$(BUILD)/bin/ld -o $@ $<

sc59x:		sc59x.o
		$(BUILD)/bin/ld -o $@ $<

sc60:		sc60.o
		$(BUILD)/bin/ld -o $@ $<

sc61:		sc61.o
		$(BUILD)/bin/ld -o $@ $<

sc62:		sc62.o
		$(BUILD)/bin/ld -o $@ $<

sc63:		sc63.o
		$(BUILD)/bin/ld -o $@ $<

%.o:		%.s
		$(BUILD)/bin/as -o $@ $<

clean:
		rm -f *~ $(OBJS) $(BINS)
