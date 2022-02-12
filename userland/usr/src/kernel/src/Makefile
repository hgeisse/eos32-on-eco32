#
# Makefile for EOS32 kernel
# selfhosting

STARTINC = start.h
STARTSRC = start.s
KERNINC = acc.h alloc.h bio.h c.h clock.h fio.h iget.h \
          klib.h machdep.h malloc.h mem.h nami.h \
          partab.h pipe.h prim.h rdwri.h \
          idedsk.h serdsk.h sdcdsk.h ramdsk.h \
          trm.h sig.h slp.h subr.h sys.h sys1.h sys2.h sys3.h sys4.h \
          sysent.h tt.h txt.h trap.h ureg.h xtest.h
KERNSRC = acc.c alloc.c bio.c c.c clock.c fio.c iget.c \
          klib.c machdep.c main.c malloc.c mem.c nami.c \
          partab.c pipe.c prim.c rdwri.c \
          idedsk.c serdsk.c sdcdsk.c ramdsk.c \
          trm.c sig.c slp.c subr.c sys.c sys1.c sys2.c sys3.c sys4.c \
          sysent.c tt.c txt.c trap.c ureg.c
LOGINC = logfile.h
LOGSRC = logfile.c
CONINC = console.h display.h keyboard.h
CONSRC = console.c display.s keyboard.s kbdtbls.s

INC = $(STARTINC) $(KERNINC) $(LOGINC) $(CONINC)
SRC = $(STARTSRC) $(KERNSRC) $(LOGSRC) $(CONSRC)
EXE = eos32
BIN = eos32.bin
MAP = eos32.map

.PHONY:		all install run clean

all:		$(BIN)

install:	$(BIN)
		-mkdir ../stdalone
		cp $(BIN) ../stdalone
		cp $(MAP) ../stdalone

# NOTE: there is no ~spoon~ sim
#run:		$(BIN)
#		sim -i -c -s 1 -t 0 \
#		  -d $(BUILD)/run/disk.img \
#		  -l $(BIN) -a 0x10000 -o run.log

$(BIN):		$(EXE)
		load -p $(EXE) $(BIN)

$(EXE):		$(INC) $(SRC) icode/icode.dump
		lcc -A \
		  -Wo-nostdinc -Wo-nostdlib \
		  -Wo-ldscript=stdalone.lnk \
		  -Wo-ldmap=$(MAP) -o $(EXE) $(SRC)

icode/icode.dump:
		make -C icode all

clean:
		make -C icode clean
		rm -f *~ $(EXE) $(BIN) $(MAP) run.log
